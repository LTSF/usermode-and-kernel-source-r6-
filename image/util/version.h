#pragma once

#include <ntifs.h>
#include <cstdint>

#include "defines.h"

enum WINDOWS_VERSION
{
	VERSION_UNKNOWN = 0,
	VERSION_1803 = 1803,
	VERSION_1809 = 1809,
	VERSION_1903 = 1903,
	VERSION_1909 = 1909,
};

ULONG get_titan_system_windows_versionn( );
WINDOWS_VERSION get_windows_version2( );