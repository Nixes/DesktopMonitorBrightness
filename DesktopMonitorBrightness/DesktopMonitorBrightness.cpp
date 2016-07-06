// DesktopMonitorBrightness.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


// refs with good leads:
// - http://stackoverflow.com/questions/26541484/enumdisplaymonitors-callback
// - http://stackoverflow.com/questions/34091968/how-to-use-getmonitorcapabilities-and-getmonitorbrightness-functions
// - https://msdn.microsoft.com/en-us/library/windows/desktop/dd692950(v=vs.85).aspx

std::vector<HANDLE>  physicalMonitorHandles;

std::vector<float>  monitorBrightnessScaleFactor;


// pysmonitor must be a physical monitor as obtained from, GetNumberOfPhysicalMonitorsFromHMONITOR and not a HMONITOR
void PrintMonitorBrightness(HANDLE physmonitor) {
	DWORD min = 0;
	DWORD current = 0;
	DWORD max = 0;

	bool bSuccess = GetMonitorBrightness(physmonitor, &min, &current, &max);

	printf("Monitor Brightness values {min %d, current %d, max %d }\n", min, current, max);
}

void AddMonitorScaleFactor(HANDLE physmonitor) {
	DWORD min = 0;
	DWORD current = 0;
	DWORD max = 0;
	float brightnessScaleFactor = 0;

	bool bSuccess = GetMonitorBrightness(physmonitor, &min, &current, &max);
	if (bSuccess) {
		brightnessScaleFactor = (float)max / 100;
		printf("Monitor Brightness values {min %d, current %d, max %d, scalefactor %g }\n", min, current, max,brightnessScaleFactor);
		monitorBrightnessScaleFactor.push_back(brightnessScaleFactor);
	}
	else {
		printf("failed to obtain monitor brightness settings");
	}
}


// this gets called each time EnumDisplayMonitors has another monitor to process
static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {
	printf("monitor callback run\n");

	// these variables are intermediaries for GetNumberOfPhysicalMonitorsFromHMONITOR
	DWORD numberPhysicalMonitors;
	LPPHYSICAL_MONITOR pPhysicalMonitors = NULL;


	// Get the number of physical monitors.
	BOOL bSuccess = GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &numberPhysicalMonitors);

	if (bSuccess) {
		printf("got some bloody monitors\n");

		// this allocates space for the physical monitor handlers based on the number of physical monitors detected
		pPhysicalMonitors = (LPPHYSICAL_MONITOR)malloc(numberPhysicalMonitors* sizeof(PHYSICAL_MONITOR));

		if (pPhysicalMonitors != NULL) {
			printf("found %d physical monitors",numberPhysicalMonitors);
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
		AddMonitorScaleFactor(monitor);
	}
}


void setBrightness(HANDLE hMonitor, int brightness, float scalefactor) {
	int calculatedBrightness = (float)brightness * scalefactor;
	DWORD dwNewBrightness = (DWORD)calculatedBrightness;
	SetMonitorBrightness(hMonitor, dwNewBrightness);
}

void setAllMonitorsBrightness(int brightness) {
	// will set the brightness of all detected monitors
	printf("Setting all monitor brightness to: %i %",brightness);

	// iterate the vector of monitor handles, and for each monitor set the brightness
	for (int i = 0; i < physicalMonitorHandles.size();i++) {
		setBrightness(physicalMonitorHandles[i], brightness, monitorBrightnessScaleFactor[i]);
	}
}


int _tmain(int argc, _TCHAR* argv[]) {

	getMonitorHandles();

	if (argc > 1) {
		setAllMonitorsBrightness((int)argv[0]);
	}
	//return 0;
}
