// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN // removes headers that I won't be touching anyway
#include <stdio.h>
#include <tchar.h>

#include <sstream> // used for argument parsing
#include <fstream> // used for config read / writing
#include <ostream>
#include <vector>

// for doing day cycle calcs
#include <ctime> // used for dealing with system time
#include <math.h> // used for sin functon
#define PI 3.14159265f // can't belive we still need to do this in 2016

// windows specific
#include <windows.h> // needed for windows types
#include <HighLevelMonitorConfigurationAPI.h> // for using window api to get and set brightness


#include "json.hpp" // from https://github.com/nlohmann/json