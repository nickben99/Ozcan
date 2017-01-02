//SystenWindows.h - useful defines

#ifndef _SystemWindows_h_
#define _SystemWindows_h_

#define SPRINTF sprintf_s

#define VSPRINTF vsprintf_s

#define SSCANF sscanf_s

#define SSCANF_STR_PARAM(theStringParamPointer, theStringLength) theStringParamPointer, theStringLength

#define FOPEN(filePointer, fileName, readWriteMode) fopen_s(&filePointer, fileName, readWriteMode);

#define STRCAT(tempPath, kStringLen, tempFilename) strcat_s(tempPath, kStringLen, tempFilename);

inline void ErrorOutput(const char* text) {
	MessageBox(0,text,"ERROR",MB_OK|MB_ICONEXCLAMATION);
}

#endif // ifndef _SystemWindows_