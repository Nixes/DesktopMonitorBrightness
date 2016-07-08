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

void PrintMonitorBrightness(HANDLE physmonitor);

bool AddMonitorScaleFactor(HANDLE physmonitor);

void GetMonitorHandles();

void SetBrightness(HANDLE hMonitor, int brightness, float scalefactor);

void SetAllMonitorsBrightness(int brightness);

void SetBrightnessFade(int targetBrightness, int initialBrightness);

float GetFloatHoursNow();

float GetSunTimeRatio(settings current_settings);

void SetBasedOnTimeOfDay(settings current_settings);

std::string LoadTextFile(std::string inFileName);

void SaveTextFile(std::string outFileName, std::string outputstring);

bool FileExists(std::string fileLocation);

void SaveSettings(settings current);

settings RestoreSettings(std::string settings_location);
