#include "stdafx.h"
#include "DesktopMonitorBrightness.h"


// refs with good leads:
// - http://stackoverflow.com/questions/26541484/enumdisplaymonitors-callback
// - http://stackoverflow.com/questions/34091968/how-to-use-getmonitorcapabilities-and-getmonitorbrightness-functions
// - https://msdn.microsoft.com/en-us/library/windows/desktop/dd692950(v=vs.85).aspx

std::vector<HANDLE>  physicalMonitorHandles;

std::vector<float>  monitorBrightnessScaleFactor;

std::vector<float>  currentMonitorBrightness;

// pysmonitor must be a physical monitor as obtained from, GetNumberOfPhysicalMonitorsFromHMONITOR and not a HMONITOR
void PrintMonitorBrightness(HANDLE physmonitor) {
	DWORD min = 0;
	DWORD current = 0;
	DWORD max = 0;

	bool bSuccess = GetMonitorBrightness(physmonitor, &min, &current, &max);

	std::cout << "Monitor Brightness values{ min: " << min << ", current: " << current << ", max: " << max << "}\n";
}

bool AddMonitorScaleFactor(HANDLE physmonitor) {
	DWORD min = 0;
	DWORD current = 0;
	DWORD max = 0;
	float brightnessScaleFactor = 0;
	float currentScaledBrightness = 0;

	bool bSuccess = GetMonitorBrightness(physmonitor, &min, &current, &max);
	if (bSuccess) {
		brightnessScaleFactor = (float)max / 100;
		currentScaledBrightness = (float)current / brightnessScaleFactor;
		std::cout << "Monitor Brightness values {min: " << min << ", current: " << current << ", max: " << max << ", scalefactor: " << brightnessScaleFactor << ", currentscaled: " << currentScaledBrightness << "}\n";

		monitorBrightnessScaleFactor.push_back(brightnessScaleFactor);
		currentMonitorBrightness.push_back(currentScaledBrightness);
		return true;
	}
	else {
		std::cout << "Failed to obtain monitor brightness settings!\n This can be caused by querying the current brightness of a given display too soon after it was last read or changed. Or by monitors that don't support this property.\n";
		return false;
	}
}


// this gets called each time EnumDisplayMonitors has another monitor to process
static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {
	// these variables are intermediaries for GetNumberOfPhysicalMonitorsFromHMONITOR
	DWORD numberPhysicalMonitors;
	LPPHYSICAL_MONITOR pPhysicalMonitors = NULL;


	// Get the number of physical monitors.
	BOOL bSuccess = GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &numberPhysicalMonitors);

	if (bSuccess) {
		// this allocates space for the physical monitor handlers based on the number of physical monitors detected
		pPhysicalMonitors = (LPPHYSICAL_MONITOR)malloc(numberPhysicalMonitors* sizeof(PHYSICAL_MONITOR));

		if (pPhysicalMonitors != NULL) {
			printf("Found %d physical monitors in this HMONITOR\n",numberPhysicalMonitors);
		} else {
			std::cout << "no physical monitors found or could not be counted\n";
		}
	}
	else {
		std::cout << "failed to get any monitors ";
		printf("error: %d \n", GetLastError());
	}

	if (bSuccess) {
		bSuccess = GetPhysicalMonitorsFromHMONITOR(hMon, numberPhysicalMonitors, pPhysicalMonitors);
		if (bSuccess) {
			printf("Physical monitor: '%s' (handle = 0x%X)\n", pPhysicalMonitors[0].szPhysicalMonitorDescription, pPhysicalMonitors[0].hPhysicalMonitor);

			// adds the monitor handle to the vector
			physicalMonitorHandles.push_back(pPhysicalMonitors[0].hPhysicalMonitor);
		}
	}

	// we always return true to signal that we want to keep looking
	return true;
}

void GetMonitorHandles() {
	// this function is an odd beast
	EnumDisplayMonitors(0, 0, MonitorEnum, 0);
	// while this function does not return anything the results are found in physicalMonitorHandles

	for each (HANDLE monitor in physicalMonitorHandles) {
		if (!AddMonitorScaleFactor(monitor)) {
			exit(EXIT_FAILURE);
		}
	}
}


void SetBrightness(HANDLE hMonitor, int brightness, float scalefactor) {
	int calculatedBrightness = (float)brightness * scalefactor;
	DWORD dwNewBrightness = (DWORD)calculatedBrightness;
	SetMonitorBrightness(hMonitor, dwNewBrightness);
}

void SetAllMonitorsBrightness(int brightness) {
	// will set the brightness of all detected monitors
	std::cout << "Setting all monitor brightness to: " << brightness << "\n";
	// iterate the vector of monitor handles, and for each monitor set the brightness
	for (unsigned int i = 0; i < physicalMonitorHandles.size();i++) {
		// according to ms docs, this takes 50ms to return
		SetBrightness(physicalMonitorHandles[i], brightness, monitorBrightnessScaleFactor[i]);
		currentMonitorBrightness[i] = brightness;
	}
}

// this is a function that attempts to fade between the current brightness setting and the target brightness setting.
// no delay required given it takes 50ms per screen to set its brightness, thus TAKES LONGER THE MORE SCREENS CONNECTED
void SetBrightnessFade(int targetBrightness, int initialBrightness) {
	// only update if the current brightness is not the same as the desired brightmess
	if (targetBrightness != initialBrightness) {
		for (int currentBrightness = initialBrightness; currentBrightness != targetBrightness;) {
			if (currentBrightness < targetBrightness) {
				currentBrightness++;
			}
			else if (currentBrightness > targetBrightness) {
				currentBrightness--;
			}
			SetAllMonitorsBrightness(currentBrightness);
		}
	}
}

float GetFloatHoursNow() {
	// current time
	std::time_t now = std::time(NULL);
	struct tm tm_now;
	float currTimeHours = 0;
	try {
		localtime_s(&tm_now,&now);
		currTimeHours = (float)tm_now.tm_hour + ((float)tm_now.tm_min / 60.0);
	} catch (int e) {
		std::cout << "An exception occurred. Exception: " << e << "\n";
	}
	return currTimeHours;
}

float GetSunTimeRatio(settings current_settings) {
	// time range between sunerise and sunset
	float lightrange = current_settings.sunset - current_settings.sunrise;

	float currenttime = GetFloatHoursNow();
	// value betweem 1 and zero defining amounnt of day progressed
	float ratio = (currenttime - current_settings.sunrise) / lightrange;
	// clamp the get float hours to min and max
	if (ratio < 0) {
		ratio = 0;
	}
	else if (ratio > 1) {
		ratio = 1;
	}
	std::cout << "DEBUG: Set Time Of Day {sunrisetime: " << current_settings.sunrise 
		<< ", sunsettime: " << current_settings.sunset
		<< ", lightrange: " << lightrange 
		<< ", currenttime: " << currenttime 
		<< ", ratio: " << ratio << "}\n";
	return ratio;
}

void SetBasedOnTimeOfDay(settings current_settings) {
	while (true) {
		int sineresult = round(sin(GetSunTimeRatio(current_settings)  * PI) * 100);
		std::cout << "Sine func result: " << sineresult << "\n";

		SetBrightnessFade(sineresult, currentMonitorBrightness[0]);
		Sleep(1000 * current_settings.polling_time);
	}
}

std::string LoadTextFile(std::string inFileName) {
	std::ifstream inFile;
	inFile.open(inFileName);

	std::stringstream strStream;
	strStream << inFile.rdbuf(); // read file into stringstream
	std::string str = strStream.str();// then convert stringstream into a real string
	inFile.close();

	return str;
}

void SaveTextFile(std::string outFileName,std::string outputstring) {
	std::ofstream outFile;
	outFile.open(outFileName);
	
	outFile << outputstring;
	outFile.close();
}

bool FileExists(std::string fileLocation) {
	std::ifstream inFile(fileLocation);
	return inFile.good();
}

// unused until a gui is implemented
// convert from settings struct to json file
void SaveSettings(settings current) {
	json j;

	j["sunrise"] = current.sunrise;
	j["sunset"] = current.sunset;
	j["polling_time"] = current.polling_time;
	j["max_global_brightness"] = current.max_global_brightness;
	j["min_global_brightness"] = current.min_global_brightness;

	std::string settingsraw = j.dump();

	SaveTextFile("settings.json", settingsraw);
	// then write string to file
}

// convert from json file to settings struct
settings RestoreSettings(std::string settings_location) {
	// provide some defaults
	settings new_settings{ 7.00 ,14.00 ,60 ,0,100 };


	if (FileExists(settings_location)) {
		std::string configfileraw = LoadTextFile(settings_location);
		std::cout << configfileraw;

		json j3 = json::parse(configfileraw);

		new_settings.sunrise = j3["sunrise"];
		new_settings.sunset = j3["sunset"];
		new_settings.polling_time = j3["polling_time"];
		new_settings.max_global_brightness = j3["max_global_brightness"];
		new_settings.min_global_brightness = j3["min_global_brightness"];
	}
	else {
		std::cout << "Failed to load config, making a new one based on defualts";
		SaveSettings(new_settings);
	}


	return new_settings;
}