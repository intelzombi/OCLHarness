#pragma once

#include "resource\resource.h"
#include "resource\targetver.h"
#include "GlobalDecl.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#include <Windowsx.h>

#include <malloc.h>
#include <memory.h>
#include "INIFileReader.h"
#include "HighPerformanceCounter.h"
#include "CSVLogger.h"
#include <tchar.h>
#pragma once
#pragma warning( disable : 4996 )
#include <strsafe.h>
#pragma warning( default : 4996 )



static  bool	volatile    g_sbOCLHarnessThreadRunning = false;
HANDLE						g_hOCLHarnessThreadHandle = 0;

static  bool	volatile    g_sbCarriageThreadRunning = false;
HANDLE						g_hCarriageThreadHandle = 0;

static  LONG	volatile	g_lvContinueRunningQ = 1;
