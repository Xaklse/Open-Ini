
#ifndef OINI_GLOBALS_H
#define OINI_GLOBALS_H


//Defines the namespace macros.
#define OINI_NAMESPACE_BEGIN(nameSpace) namespace nameSpace{
#define OINI_NAMESPACE_END(nameSpace) }

//Defines the solution configuration.
#if defined(_DEBUG)
	#define OINI_CONFIGURATION_DEBUG
#else
	#define OINI_CONFIGURATION_RELEASE
#endif

//Defines the solution platform.
#if defined(_WIN32) || defined(_WIN64)
	#define OINI_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
	#define OINI_PLATFORM_MACOSX
#elif defined(__linux__)
	#define OINI_PLATFORM_LINUX
#else
	#define OINI_PLATFORM_UNDETERMINED
#endif



#if defined(OINI_CONFIGURATION_DEBUG)
	//Enables macros mainly used for debugging and/or development.
	#define OINI_DEBUG_LOG_TO_DEBUGGER
	#define OINI_DEBUG_MEMORY_LEAKS
#endif

#if defined(OINI_PLATFORM_WINDOWS)
	//Turns off certain problematic macros related to header files of Windows.
	#define NOMINMAX
	#define VC_EXTRALEAN
	#define WINDOWS_LEAN_AND_MEAN
#endif



//Includes the static array container of the Standard Template Library (STL).
#include <array>

//Includes input and output functionality using abstract streams.
#include <iostream>

//Includes general utilities to manage memory.
#include <memory>

//Includes functions for memory management, random number generation,
//communication with the environment, integer arithmetics, searching, sorting
//and converting.
#include <stdlib.h>

//Includes the string class of the Standard Template Library (STL).
#include <string>

//Includes the dynamic array container of the Standard Template Library (STL).
#include <vector>

#if defined(OINI_DEBUG_MEMORY_LEAKS) && defined(OINI_PLATFORM_WINDOWS)
	//Keeps track of memory allocation and deallocation.
	#include <crtdbg.h>
#endif



//Includes some Boost header files.
#include "Boost/algorithm/clamp.hpp"
#include "Boost/lexical_cast.hpp"
#include "Boost/utility.hpp"



#if defined(OINI_PLATFORM_WINDOWS)
	//Fixes missing definition of the "GetModuleFileName" macro.
	#define POCO_NO_UNWINDOWS
#endif

//Includes some POCO header files.
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/NumberFormatter.h"
#include "Poco/StopWatch.h"
#include "Poco/String.h"
#include "Poco/Types.h"



OINI_NAMESPACE_BEGIN(Oini)

typedef Poco::Int8 int8;
typedef Poco::Int16 int16;
typedef Poco::Int32 int32;
typedef Poco::Int64 int64;

typedef unsigned int uint;
typedef Poco::UInt8 uint8;
typedef Poco::UInt16 uint16;
typedef Poco::UInt32 uint32;
typedef Poco::UInt64 uint64;

typedef float float32;
typedef double float64;

typedef std::string string;


static string Str(bool data)
{
	return data ? "true" : "false";
}

static string Str(float data, uint fractionalDigits = 3u)
{
	return Poco::NumberFormatter::format(data, fractionalDigits);
}

template <typename T>
static string Str(const T& data)
{
	return boost::lexical_cast<string>(data);
}

OINI_NAMESPACE_END(Oini)



#if defined(OINI_DEBUG_MEMORY_LEAKS) && defined(OINI_PLATFORM_WINDOWS)
	#define OINI_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
	#define OINI_NEW new
#endif

#define OINI_APP Oini::Application::Get
#define OINI_FILE Str(__FILE__).substr((Str(__FILE__).find_last_of("/\\")) + 1)
#define OINI_INFO string() + "File: " + OINI_FILE + "; Line: " + Str(__LINE__)
#define OINI_LOG OINI_APP()->Log


#endif //OINI_GLOBALS_H
