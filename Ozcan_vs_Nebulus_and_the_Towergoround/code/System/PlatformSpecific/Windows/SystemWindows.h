//SystenWindows.h - useful defines

#ifndef _SystemWindows_h_
#define _SystemWindows_h_

#include <windows.h> // for strncpy_s
#include <float.h> // for _isnan

#define SPRINTF sprintf_s

#define VSPRINTF vsprintf_s

#define SSCANF sscanf_s

#define SSCANF_STR_PARAM(theStringParamPointer, theStringLength) theStringParamPointer, theStringLength

#define FOPEN(filePointer, fileName, readWriteMode) fopen_s(&filePointer, fileName, readWriteMode);

#define STRCAT(targetBuffer, targetBufferLength, fromBuffer) strcat_s(targetBuffer, targetBufferLength, fromBuffer);

inline void ErrorOutput(const char* text) {
	MessageBox(0,text,"ERROR",MB_OK|MB_ICONEXCLAMATION);
}

inline bool PlatformSpecificIsNaN(const float& num) {
	return 0 != _isnan(num);
}

inline void PlatformSpecificStrncpy(char* copyTo, int copyToSize, const char* copyFrom, int numToCopy) {
	strncpy_s(copyTo, copyToSize, copyFrom, numToCopy);
}

inline void PlatformSpecificReverseString(char* buffer) {
	_strrev(buffer); // reverse the latest line of file
}

inline void PlatformSpecificCopyString(char* copyTo, const char* copyFrom, int copyToSize) {
	strcpy_s(copyTo, copyToSize, copyFrom);
}

inline void PlatformSpecificSleep(unsigned milliseconds) {
	Sleep(milliseconds);
}

#endif // ifndef _SystemWindows_