// SystemInterface.h

//system includes---
//------------------

#ifndef _SystemInterface_h_
#define _SystemInterface_h_

#ifdef _WINDOWS
#include "System/PlatformSpecific/Windows/SystemWindows.h"
#elif OSX
#include "System/PlatformSpecific/OSX/SystemOSX.h"
#endif

#endif // ifndef _SystemInterface_h_