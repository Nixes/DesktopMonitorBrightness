// DesktopMonitorBrightness.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


// refs with good leads:
// - http://stackoverflow.com/questions/26541484/enumdisplaymonitors-callback
// - http://stackoverflow.com/questions/34091968/how-to-use-getmonitorcapabilities-and-getmonitorbrightness-functions
// - https://msdn.microsoft.com/en-us/library/windows/desktop/dd692950(v=vs.85).aspx

std::vector<HANDLE>  hMonitors;


// pysmonitor must be a physical monitor as obtained from, GetNumberOfPhysicalMonitorsFromHMONITOR and not a HMONITOR
void PrintMonitorBrightness(HANDLE physmonitor) {
	DWORD min = 0;
	DWORD current = 0;
	DWORD max = 0;

	bool bSuccess = GetMonitorBrightness(physmonitor, &min, &current, &max);

	//printf("Monitor Brightness values {min %d, current %d, max %d }\n", pdwMinimumBrightness, pdwCurrentBrightness, pdwMaximumBrightness);
}


// this gets called each time EnumDisplayMonitors gets called.
static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {
	printf("monitor callback run\n");

	// these variables are intermediaries for 
	DWORD cPhysicalMonitors;
	LPPHYSICAL_MONITOR pPhysicalMonitors = NULL;


	// Get the number of physical monitors.
	BOOL bSuccess = GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &cPhysicalMonitors);

	if (bSuccess) {
		printf("got some bloody monitors\n");

		// get the physical monitors from the raw data?
		pPhysicalMonitors = (LPPHYSICAL_MONITOR)malloc(cPhysicalMonitors* sizeof(PHYSICAL_MONITOR));

		if (pPhysicalMonitors != NULL) {
			// focus here <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			wprintf(L"Physical monitor: '%s' (handle = 0x%X)\n", pPhysicalMonitors[0].szPhysicalMonitorDescription, pPhysicalMonitors[0].hPhysicalMonitor);

			// adds the monitor handle to the vector
			hMonitors.push_back(pPhysicalMonitors[0].hPhysicalMonitor);

			PrintMonitorBrightness(pPhysicalMonitors[0].hPhysicalMonitor);
		} else {
			printf("but physical monitors were null\n");
		}
	}
	else {
		printf("failed to get any monitors ");
		printf("error: %d \n", GetLastError());
	}

	// we return true to signal that we want to keep looking
	return true;
}

void getMonitorHandle() {
	//HWND hWnd;

	// Get the monitor handle.

	// this only gets the monitor of the current window, this is not helpfull
	// hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);

	// this function is an odd beast
	EnumDisplayMonitors(0, 0, MonitorEnum, 0); // unsure what this end bit is doing, requires some testing

	/*for each (HANDLE monitor in hMonitors) {
		PrintMonitorBrightness(monitor);

	}*/

}


void setBrightness(HMONITOR hMonitor, int brightness) {
	DWORD dwNewBrightness = (DWORD)brightness;
	SetMonitorBrightness(hMonitor, dwNewBrightness);
}

void setAllBrightness(int brightness) {
	// will set the brightness of all detected monitors

	// iterate the vector of monitor handles, and for each monitor set the brightness
	for each (HMONITOR monitor in hMonitors) {
		setBrightness(monitor, brightness);
	}
}


int _tmain(int argc, _TCHAR* argv[]) {
	getMonitorHandle();
	//return 0;
}
