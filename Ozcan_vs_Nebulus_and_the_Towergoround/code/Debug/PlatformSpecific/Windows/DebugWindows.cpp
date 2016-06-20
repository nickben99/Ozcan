// DebugWindows.cpp

#if _DEBUG

#ifdef _WINDOWS

//--- System includes ---------------
#include <Windows.h>
//----------------------------------

#include "FileReading/CTextFileReader.h" // for writing to files

//external variables --
extern HDC hDC;
//---------------------

char staticBuffer[500] = "STATIC DEBUG::";
char buffer[500] = "DEBUG::";

//--- Header files ---------
#include "DebugWindows.h" // the header file for this class
//--------------------------

void DebugImplementation::staticDebug(char * thisString)
{
	// stop writting to out of bounds memory
	if (strlen(staticBuffer)+strlen(thisString) < 500){
		strcat_s(staticBuffer, thisString);} // add to buffer
}

void DebugImplementation::printStaticDebug()
{
    // set the color
    SetTextColor(hDC, RGB(255, 0, 0));
    //set the Bk mode and colour 
    SetBkMode(hDC, OPAQUE);
	SetBkColor(hDC, RGB(0, 0, 0));
    // print the message
    TextOut(hDC, 0, 20, staticBuffer, strlen(staticBuffer));
}

void DebugImplementation::debug(char * thisString)
{
	// stop writting to out of bounds memory
	if (strlen(buffer)+strlen(thisString) < 500){
		strcat_s(buffer, thisString);} // add to buffer
}

void DebugImplementation::printDebug()
{
    // set the color
    SetTextColor(hDC,RGB(255, 0, 0));
    //set the Bk mode and colour
    SetBkMode(hDC, OPAQUE);
	SetBkColor(hDC, RGB(0, 0, 0));
    // print the message
    TextOut(hDC, 0, 300, buffer, strlen(buffer));
	// reset the buffer
	memset( buffer, ' ',  strlen(buffer));
	strcpy_s(buffer, "DEBUG::");
} 

// send debug string to output file
void DebugImplementation::outputToFile(char * thisString)
{
	/*	static file handle for writing to files (ios::out compleatly replaces
		any contents already in the file */
	static CTextFileReader fileWriter; 

	if (!fileWriter.inFile.is_open()){ 
		fileWriter.openFile("debug.txt", ios::out);}

	fileWriter.writeTextToFile(thisString); // write the output to the file
}

#endif

#endif // #if _DEBUG