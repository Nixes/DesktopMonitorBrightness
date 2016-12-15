struct settings {
	// used for automatic calculation of sunrise/sunset times
	float longitude;
	float latitude;

	// if true tells the class to generate sunrise and sunset times automatically
	bool auto_suntime_calc;

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
	void ResetMonitorHandles();
	void GetMonitorHandles();
	// pysmonitor must be a physical monitor as obtained from, GetNumberOfPhysicalMonitorsFromHMONITOR and not a HMONITOR
	void PrintMonitorBrightness(HANDLE physmonitor);
	void SetBrightness(HANDLE hMonitor, int brightness, float scalefactor);
	float GetFloatHoursNow();
	bool AddMonitorScaleFactor(HANDLE physmonitor);


	// start file manip functions
	std::string LoadTextFile(std::string inFileName);

	bool SaveTextFile(std::string outFileName, std::string outputstring);

	bool FileExists(std::string fileLocation);
	// end file manip functions

	float GetSunTimeRatio(float currenttime);
public:
	const int GetBrightness();
	bool Tests(std::string & error);
	DesktopMonitorManager();

	static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM dwData);
	void SetAllMonitorsBrightness(int brightness);
	void SetBrightnessFade(int targetBrightness);
	float SetBasedOnTimeOfDay();

	bool SaveSettings();
	void AutoUpdateSuntime();
	void RestoreSettings(std::string settings_location);

	// bunch of generic getters and setters
	void SetSunrisetime(float sunrisetime);
	void SetSunsettime(float sunsettime);
	void SetPollingTime(int polling_time);
	void SetMaxBrightness(int max_brightness);
	void SetMinBrightness(int min_brightness);

	const float GetSunrisetime();
	const float GetSunsettime();
	const int GetPollingTime();
	const int GetMaxBrightness();
	const int GetMinBrightness();
	const double GetLongitude();
	const double GetLatitude();
};