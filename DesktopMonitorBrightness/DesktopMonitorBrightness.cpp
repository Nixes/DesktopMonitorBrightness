// DesktopMonitorBrightness.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


std::vector<HMONITOR>  hMonitors;

// this gets called each time EnumDisplayMonitors gets called.
static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {
	// adds the monitor handle to the vector
	//hMonitors.push_back(hMon);
	return true;
}

void getMonitorHandle() {
	HMONITOR hMonitor = NULL;
	DWORD cPhysicalMonitors;
	LPPHYSICAL_MONITOR pPhysicalMonitors = NULL;

	//HWND hWnd;

	// Get the monitor handle.

	// this only gets the monitor of the current window, this is not helpfull
	// hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
	EnumDisplayMonitors(0, 0, MonitorEnum, (LONG)&hMonitor);

	// Get the number of physical monitors.
	BOOL bSuccess = GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor,&cPhysicalMonitors);

	if (bSuccess) {
		printf("got some bloody monitors");
	}
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
	//EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)this);
	return 0;
}
