#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2021
//
// www.lekseecon.nl
//

// Including SDKDDKVer.h defines the highest available Windows platform.



// Windows 7 +

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#ifndef WINVER
#ifdef _WIN32_WINNT
// set WINVER based on _WIN32_WINNT
#define WINVER          _WIN32_WINNT
#else
#define WINVER          0x0601
#endif
#endif
#define NTDDI_VERSION NTDDI_WIN7


