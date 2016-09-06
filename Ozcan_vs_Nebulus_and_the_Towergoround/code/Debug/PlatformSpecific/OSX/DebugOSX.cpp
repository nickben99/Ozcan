// DebugWindows.cpp

#if _DEBUG

#ifdef OSX

//--- System includes ---------------
//----------------------------------

#include "FileReading/CTextFileReader.h" // for writing to files

//external variables --
//---------------------

char staticBuffer[500] = "STATIC DEBUG::";
char buffer[500] = "DEBUG::";

//--- Header files ---------
#include "DebugOSX.h" // the header file for this class
//--------------------------

void DebugImplementation::staticDebug(char*)
{
}

void DebugImplementation::printStaticDebug()
{
}

void DebugImplementation::debug(char * thisString)
{
}

void DebugImplementation::printDebug()
{
} 

// send debug string to output file
void DebugImplementation::outputToFile(char * thisString)
{
}

#endif // #ifdef OSX

#endif // #if _DEBUG