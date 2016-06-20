// DebugWindows.h

#if _DEBUG

#ifdef _WINDOWS

#ifndef _DebugWindows_H_
#define _DebugWindows_H_

//--- System Includes -----------
//-------------------------------

//--- Header files --------------
//-------------------------------

//--- external variables --------
//-------------------------------

class DebugImplementation
{
	private:

	public:
		void staticDebug(char * debugString);
		void printStaticDebug();
		void debug(char * thisString);
		void printDebug();
		void outputToFile(char * thisString);
};

#endif

#endif

#endif // #if _DEBUG