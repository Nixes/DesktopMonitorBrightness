#include "stdafx.h"
#include "DesktopMonitorBrightness.h"


// refs with good leads:
// - http://stackoverflow.com/questions/26541484/enumdisplaymonitors-callback
// - http://stackoverflow.com/questions/34091968/how-to-use-getmonitorcapabilities-and-getmonitorbrightness-functions
// - https://msdn.microsoft.com/en-us/library/windows/desktop/dd692950(v=vs.85).aspx

// some class design reminders:
// - all getters should be marked as const

// I really dislike having this here rather than it being private inside the desktopmonitormanager class
// but I can't figure a way to work around handler class methods
std::vector<HANDLE>  physicalMonitorHandles;


DesktopMonitorManager::DesktopMonitorManager() {
	RestoreSettings("settings.json");

	GetMonitorHandles();
}

void DesktopMonitorManager::GetMonitorHandles() {
	// this function is an odd beast
	::EnumDisplayMonitors(NULL, NULL, MonitorEnum, 0);
	// while this function does not return anything the results are found in physicalMonitorHandles

	for each (HANDLE monitor in physicalMonitorHandles) {
		if (!AddMonitorScaleFactor(monitor)) {
			exit(EXIT_FAILURE);
		}
	}
}

// pysmonitor must be a physical monitor as obtained from, GetNumberOfPhysicalMonitorsFromHMONITOR and not a HMONITOR

 void DesktopMonitorManager::PrintMonitorBrightness(HANDLE physmonitor) {
	DWORD min = 0;
	DWORD current = 0;
	DWORD max = 0;

	bool bSuccess = GetMonitorBrightness(physmonitor, &min, &current, &max);

	std::cout << "Monitor Brightness values{ min: " << min << ", current: " << current << ", max: " << max << "}\n";
}

 void DesktopMonitorManager::SetBrightness(HANDLE hMonitor, int brightness, float scalefactor) {
	int calculatedBrightness = (float)brightness * scalefactor;
	DWORD dwNewBrightness = (DWORD)calculatedBrightness;
	SetMonitorBrightness(hMonitor, dwNewBrightness);
}

 float DesktopMonitorManager::GetFloatHoursNow() {
	// current time
	std::time_t now = std::time(NULL);
	struct tm tm_now;
	float currTimeHours = 0;
	try {
		localtime_s(&tm_now, &now);
		currTimeHours = (float)tm_now.tm_hour + ((float)tm_now.tm_min / 60.0);
	}
	catch (int e) {
		std::cout << "An exception occurred. Exception: " << e << "\n";
	}
	return currTimeHours;
}

//
// member funcs
//

 bool DesktopMonitorManager::AddMonitorScaleFactor(HANDLE physmonitor) {
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

		current_brightness = (int)round(currentScaledBrightness);

		monitorBrightnessScaleFactor.push_back(brightnessScaleFactor);
		return true;
	}
	else {
		std::cout << "Failed to obtain monitor brightness settings!\n This can be caused by querying the current brightness of a given display too soon after it was last read or changed. Or by monitors that don't support this property.\n";
		return false;
	}
}

// start file manip functions

 std::string DesktopMonitorManager::LoadTextFile(std::string inFileName) {
	std::ifstream inFile;
	inFile.open(inFileName);

	std::stringstream strStream;
	strStream << inFile.rdbuf(); // read file into stringstream
	std::string str = strStream.str();// then convert stringstream into a real string
	inFile.close();

	return str;
}

 void DesktopMonitorManager::SaveTextFile(std::string outFileName, std::string outputstring) {
	std::ofstream outFile;
	outFile.open(outFileName);

	outFile << outputstring;
	outFile.close();
}

 bool DesktopMonitorManager::FileExists(std::string fileLocation) {
	std::ifstream inFile(fileLocation);
	return inFile.good();
}


 const int DesktopMonitorManager::GetPollingTime() {
	return current_settings.polling_time;
}


// this must be public due to the callback shenanigans
// this gets called each time EnumDisplayMonitors has another monitor to process
BOOL CALLBACK DesktopMonitorManager::MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM dwData) {
	// these variables are intermediaries for GetNumberOfPhysicalMonitorsFromHMONITOR
	DWORD numberPhysicalMonitors;
	LPPHYSICAL_MONITOR pPhysicalMonitors = NULL;


	// Get the number of physical monitors.
	BOOL bSuccess = GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &numberPhysicalMonitors);

	if (bSuccess) {
		// this allocates space for the physical monitor handlers based on the number of physical monitors detected
		pPhysicalMonitors = (LPPHYSICAL_MONITOR)malloc(numberPhysicalMonitors * sizeof(PHYSICAL_MONITOR));

		if (pPhysicalMonitors != NULL) {
			printf("Found %d physical monitors in this HMONITOR\n", numberPhysicalMonitors);
		}
		else {
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
			// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< this is causing exceptions!!!
		}
	}

	// we always return true to signal that we want to keep looking
	return true;
}

 void DesktopMonitorManager::SetAllMonitorsBrightness(int brightness) {
	// will set the brightness of all detected monitors
	std::cout << "Setting all monitor brightness to: " << brightness << "\n";
	// iterate the vector of monitor handles, and for each monitor set the brightness
	for (unsigned int i = 0; i < physicalMonitorHandles.size(); i++) {
		// according to ms docs, this takes 50ms to return
		SetBrightness(physicalMonitorHandles[i], brightness, monitorBrightnessScaleFactor[i]);
	}
	current_brightness = brightness;
}

// this is a function that attempts to fade between the current brightness setting and the target brightness setting.
// no delay required given it takes 50ms per screen to set its brightness, thus TAKES LONGER THE MORE SCREENS CONNECTED

 void DesktopMonitorManager::SetBrightnessFade(int targetBrightness) {
	// only update if the current brightness is not the same as the desired brightmess
	if (targetBrightness != current_brightness) {
		while (current_brightness != targetBrightness) {
			if (current_brightness < targetBrightness) {
				current_brightness++;
			}
			else if (current_brightness > targetBrightness) {
				current_brightness--;
			}
			SetAllMonitorsBrightness(current_brightness);
		}
	}
}

 float DesktopMonitorManager::GetSunTimeRatio(float currenttime) {
	// time range between sunerise and sunset
	float lightrange = current_settings.sunset - current_settings.sunrise;

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

 void DesktopMonitorManager::SetBasedOnTimeOfDay() {
	int sineresult = round(sin(GetSunTimeRatio( GetFloatHoursNow() )  * PI) * 100);
	std::cout << "Sine func result: " << sineresult << "\n";

	SetBrightnessFade(sineresult);
}

// convert from settings struct to json file
 void DesktopMonitorManager::SaveSettings() {
	json j;

	j["sunrise"] = current_settings.sunrise;
	j["sunset"] = current_settings.sunset;
	j["polling_time"] = current_settings.polling_time;
	j["max_global_brightness"] = current_settings.max_global_brightness;
	j["min_global_brightness"] = current_settings.min_global_brightness;

	std::string settingsraw = j.dump();

	SaveTextFile("settings.json", settingsraw);
	// then write string to file
}

// convert from json file to settings struct
 void DesktopMonitorManager::RestoreSettings(std::string settings_location) {
	// provide some defaults
	current_settings = { 7.00 ,14.00 ,60 ,0,100 };


	if (FileExists(settings_location)) {
		std::string configfileraw = LoadTextFile(settings_location);
		std::cout << configfileraw;

		json j3 = json::parse(configfileraw);

		current_settings.sunrise = j3["sunrise"];
		current_settings.sunset = j3["sunset"];
		current_settings.polling_time = j3["polling_time"];
		current_settings.max_global_brightness = j3["max_global_brightness"];
		current_settings.min_global_brightness = j3["min_global_brightness"];
	}
	else {
		std::cout << "Failed to load config, making a new one based on defualts";
		SaveSettings();
	}

}

 const int DesktopMonitorManager::GetBrightness() {
	return current_brightness;
}

 bool DesktopMonitorManager::Tests(std::string &error) {
	 bool return_value = true;
	 std::ostringstream buff;

	 float value = 0;
	 // check basic ratios
	 if (value = round(sin(0.5 * PI) * 100) != 100) {
		 // failed
		 return_value = false;
		 buff << "Failed to confirm basic sine function test1: " << value << "\n";
	 }
	 if (value = round(sin(0 * PI) * 100) != 0) {
		 // failed
		 return_value = false;
		 buff << "Failed to confirm basic sine function test2: " << value << "\n";
	 }
	 if (value = round(sin(1 * PI) * 100) != 0) {
		 // failed
		 return_value = false;
		 buff << "Failed to confirm basic sine function test3: " << value << "\n";
	 }

	 // check get sun time ratio function
	 current_settings = { (float)1,(float)2,60,0,100 };
	 if (value = GetSunTimeRatio(1.5) != 0.5) {
		 return_value = false;
		 buff << "Failed to validate GetSunTimeRatio test1: "<< value << "\n";
	 }
	 if (value = GetSunTimeRatio(2) != 1) {
		 return_value = false;
		 buff << "Failed to validate GetSunTimeRatio test2: " << value << "\n";
	 }
	 if (value = GetSunTimeRatio(1) != 0) {
		 return_value = false;
		 buff << "Failed to validate GetSunTimeRatio test3: " << value << "\n";
	 }

	 error = buff.str();
	 return return_value;
 }