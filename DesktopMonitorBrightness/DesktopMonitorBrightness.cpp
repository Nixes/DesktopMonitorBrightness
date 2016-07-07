// DesktopMonitorBrightness.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


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

	printf("Monitor Brightness values {min %d, current %d, max %d }\n", min, current, max);
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
		printf("Monitor Brightness values {min %d, current %d, max %d, scalefactor %g, currentscaled %g }\n", min, current, max, brightnessScaleFactor, currentScaledBrightness);
		monitorBrightnessScaleFactor.push_back(brightnessScaleFactor);
		currentMonitorBrightness.push_back(currentScaledBrightness);
		return true;
	}
	else {
		printf("Failed to obtain monitor brightness settings!\n This can be caused by querying the current brightness of a given display too soon after it was last read or changed. Or by monitors that don't support this property.\n");
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
			printf("no physical monitors found or could not be counted\n");
		}
	}
	else {
		printf("failed to get any monitors ");
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

void getMonitorHandles() {
	// this function is an odd beast
	EnumDisplayMonitors(0, 0, MonitorEnum, 0);
	// while this function does not return anything the results are found in physicalMonitorHandles

	for each (HANDLE monitor in physicalMonitorHandles) {
		if (!AddMonitorScaleFactor(monitor)) {
			exit(EXIT_FAILURE);
		}
	}
}


void setBrightness(HANDLE hMonitor, int brightness, float scalefactor) {
	int calculatedBrightness = (float)brightness * scalefactor;
	DWORD dwNewBrightness = (DWORD)calculatedBrightness;
	SetMonitorBrightness(hMonitor, dwNewBrightness);
}

void setAllMonitorsBrightness(int brightness) {
	// will set the brightness of all detected monitors
	printf("Setting all monitor brightness to: %i % \n",brightness);

	// iterate the vector of monitor handles, and for each monitor set the brightness
	for (unsigned int i = 0; i < physicalMonitorHandles.size();i++) {
		// according to ms docs, this takes 50ms to return
		setBrightness(physicalMonitorHandles[i], brightness, monitorBrightnessScaleFactor[i]);
		currentMonitorBrightness[i] = brightness;
	}
}

// this is a function that attempts to fade between the current brightness setting and the target brightness setting.
// no delay required given it takes 50ms per screen to set its brightness, thus TAKES LONGER THE MORE SCREENS CONNECTED
void SetBrightnessFade(int targetBrightness, int initialBrightness) {
	for (int currentBrightness = initialBrightness; currentBrightness != targetBrightness;) {
		if (currentBrightness < targetBrightness) {
			currentBrightness++;
		}
		else if (currentBrightness > targetBrightness) {
			currentBrightness--;
		}
		setAllMonitorsBrightness(currentBrightness);
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
		printf( "An exception occurred. Exception Nr. d% \n",e);
	}
	return currTimeHours;
}

float GetSunTimeRatio() {
	// sunrise in 24 decimal hour time
	float sunrisetime_hours = 7.00;
	// sunset in 24 decimal hour time
	float sunsettime_hours = 17.00;
	// time range between the two
	float lightrange = sunsettime_hours - sunrisetime_hours;

	float currenttime = GetFloatHoursNow();
	// value betweem 1 and zero defining amounnt of day progressed
	float ratio = (currenttime - sunrisetime_hours) / lightrange;
	// clamp the get float hours to min and max
	if (ratio < 0) {
		ratio = 0;
	}
	else if (ratio > 1) {
		ratio = 1;
	}

	// now calculate sine based on this

	printf("DEBUG: Set Time Of Day {sunrisetime: %g, sunsettime %g, lightrange: %g, currenttime: %g, ratio: %g}\n", sunrisetime_hours, sunsettime_hours, lightrange, currenttime, ratio);
	return ratio;
}

void SetBasedOnTimeOfDay(int polling_time_secs) {
	while (true) {
		int sineresult = round(sin(GetSunTimeRatio()  * PI) * 100);
		printf("Sine func result: %i\n", sineresult);

		SetBrightnessFade(sineresult, currentMonitorBrightness[0]);
		Sleep(1000 * polling_time_secs);
	}
}


int main(int argc, const char* argv[]) {
	printf("DesktopMonitorBrightness, to use include arg1 brightness as a value between 0 and 100\n");

	getMonitorHandles();

	//printf("Float hours %g \n", GetFloatHoursNow());
	SetBasedOnTimeOfDay(10);

	if (argc > 1) {
		std::istringstream ss(argv[1]);

		int brightness;
		if (ss >> brightness) { // checks to make sure conversion to integer was valid
			//setAllMonitorsBrightness(brightness);
			SetBrightnessFade(brightness, currentMonitorBrightness[0]);
		} else {
			printf("Unable to parse brightness argument!\n");
			return 1;
		}
	}
	return 0;
}
