#pragma once

//
// Nico Kaan
// The Netherlands
// copyright (c) 2007-2026
//
// www.lekseecon.nl
//

// stdafx.hpp : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#include "condcomp.hpp"

// minimum platform definitions:
#include "targetver.hpp"

#define WIN32_LEAN_AND_MEAN  // exclude APIs such as Cryptograpfor TextFileReaderW
#include <windows.h>

// C related files

#include <tchar.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1   // For TextFileReaderW
#include <winsock2.h>
#include <shlobj.h>
#include <wininet.h>
#include <crtdbg.h>      // For _ASSERTE()

// C++ related files
#include <cstdint>
#include <string>
#include <array>
#include <cassert>
#include <charconv>
#include <bit>
#include <condition_variable>
#include <exception>
#include <filesystem>
#include <fstream>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <codecvt>
#include <list>
#include <numeric>
#include <optional>
#include <queue>
#include <regex>
#include <random>
#include <set>
#include <vector>
#include <numbers>
#include <expected>

#include <nlohmann/json.hpp>
#include <gsl/gsl>


#include "constants_char_string.hpp"
#include "constants.hpp"
#include "txtfileio.hpp"
#include "exceptions.hpp"
#include "winapi.hpp"
#include "CfgPar.hpp"
#include "guibase.hpp"  
#include "versiondata.hpp"
#include "support.hpp"
#include "mypar.hpp"
#include "qcode.hpp"
#include "configuration.hpp"
#include "utf8win.hpp"
#include "BCDfies.hpp"
#include "readcfg.hpp"
#include "ssifilereader.hpp"
#include "TCPIPclients.hpp"
#include "vars.hpp"
#include "siocparameters.hpp"   
#include "varpool.hpp"         
#include "gui.hpp"              
#include "topsxclient.hpp"       
#include "frompsxclient.hpp"      
#include "siocclients.hpp" 
#include "main.hpp"
