//SystemOSX.h - useful defines

#ifndef _SystemOSX_h_
#define _SystemOSX_h_

#define SPRINTF sprintf

#define VSPRINTF vsprintf

#define SSCANF sscanf

#define SSCANF_STR_PARAM(theStringParamPointer, theStringLength) theStringParamPointer

#define FOPEN(filePointer, fileName, readWriteMode) filePointer = fopen(fileName, readWriteMode);

#define STRCAT(tempPath, kStringLen, tempFilename) strcat(tempPath, tempFilename);

inline void ErrorOutput(const char*) {

}

#endif // ifndef _SystemOSX_