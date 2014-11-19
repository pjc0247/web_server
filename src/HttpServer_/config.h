#ifndef _CONFIG_H
#define _CONFIG_H

#if defined(WIN32) || defined(_WIN32)
	#define TARGET_WIN32 1
	#include "config_win32.h"
#else
	#define TARGET_BSD 1
	#include "config_bsd.h"
#endif

#endif //_CONFIG_H