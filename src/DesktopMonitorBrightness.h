struct settings {
	// sunrise in 24 decimal hour time
	float sunrise;
	// sunset in 24 decimal hour time
	float sunset;
	// time to wait between updating brightness
	int polling_time;

	// maximum brightness to be set
	int max_global_brightness;
	// minimum brightness to be set
	int min_global_brightness;
};

class DesktopMonitorManager {
private:
	//
	// vars
	//
	settings current_settings;
	std::vector<float>  monitorBrightnessScaleFactor;
	int current_brightness;

	//
	// member funcs
	//
	void GetMonitorHandles();
	// pysmonitor must be a physical monitor as obtained from, GetNumberOfPhysicalMonitorsFromHMONITOR and not a HMONITOR
	void PrintMonitorBrightness(HANDLE physmonitor);
	void SetBrightness(HANDLE hMonitor, int brightness, float scalefactor);
	float GetFloatHoursNow();
	bool AddMonitorScaleFactor(HANDLE physmonitor);


	// start file manip functions
	std::string LoadTextFile(std::string inFileName);

	void SaveTextFile(std::string outFileName, std::string outputstring);

	bool FileExists(std::string fileLocation);
	// end file manip functions

	float GetSunTimeRatio(float currenttime);
public:
	const int GetBrightness();
	bool Tests(std::string & error);
	const int GetPollingTime();
	DesktopMonitorManager();

	//BOOL MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor);
	static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM dwData);
	void SetAllMonitorsBrightness(int brightness);
	void SetBrightnessFade(int targetBrightness);
	float SetBasedOnTimeOfDay();

	void SaveSettings();
	void RestoreSettings(std::string settings_location);
};