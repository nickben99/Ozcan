// DebugInterface.h

#if _DEBUG

#ifndef _DebugInterface_h_
#define _DebugInterface_h_

//system includes---
#include <stdarg.h> // for va_list variable length parameter list
#include <stdio.h> // for vsprintf_s
//------------------

#ifdef _WINDOWS
#include "Debug/PlatformSpecific/Windows/DebugWindows.h"
#elif OSX
#include "Debug/PlatformSpecific/OSX/DebugOSX.h"
#else
#include "Debug/PlatformSpecific/iOS/DebugiOS.h"
#endif

class DebugInterface
{ 
	public:
		void staticDebug(char * debugString, ...)
		{
			char thisString[256];
			va_list vlist; // va_list
	
			va_start(vlist, debugString); // set to beginning of argument list
#ifdef _WINDOWS
			vsprintf_s(thisString, debugString, vlist); // print formatted string to thisString
#else
			vsprintf(thisString, debugString, vlist);
#endif
            va_end(vlist); // reset argument pointer
			implementation.staticDebug(thisString);
		}

		void printStaticDebug()
		{
			implementation.printStaticDebug();
		}

		void debug(char * debugString, ...)
		{
			char thisString[256];
			va_list vlist; // va_list
	
			va_start(vlist, debugString); // set to beginning of argument list
#ifdef _WINDOWS
			vsprintf_s(thisString, debugString, vlist); // print formatted string to thisString
#else
            vsprintf(thisString, debugString, vlist); // print formatted string to thisString
#endif
			va_end(vlist); // reset argument pointer

			implementation.debug(thisString);
		}

		void printDebug()
		{
			implementation.printDebug();
		}

		void outputToFile(char * debugString, ...)
		{
			char thisString[256];
			va_list vlist; // va_list
	
			va_start(vlist, debugString); // set to beginning of argument list
#ifdef _WINDOWS
			vsprintf_s(thisString, debugString, vlist); // print formatted string to thisString
#else
            vsprintf(thisString, debugString, vlist); // print formatted string to thisString
#endif
			va_end(vlist); // reset argument pointer

			implementation.outputToFile(thisString);
		}

private:
	DebugImplementation implementation;
};

#endif // ifndef _DebugInterface_h_

#endif // #if _DEBUG