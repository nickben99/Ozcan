//SystemOSX.h - useful defines

#ifndef _SystemOSX_h_
#define _SystemOSX_h_

#include <cstdlib>
#include <string.h>
#include <unistd.h> // for usleep

#define SPRINTF sprintf

#define VSPRINTF vsprintf

#define SSCANF sscanf

#define SSCANF_STR_PARAM(theStringParamPointer, theStringLength) theStringParamPointer

#define FOPEN(filePointer, fileName, readWriteMode) filePointer = fopen(fileName, readWriteMode);

#define STRCAT(targetBuffer, targetBufferLength, fromBuffer) strcat(targetBuffer, fromBuffer);

inline void ErrorOutput(const char*) {

}

inline bool IsNaN(const float& num) {
    return std::isnan(num) || std::isinf(num);
}

inline void PlatformSpecificStrncpy(char* copyTo, int copyToSize, const char* copyFrom, int numToCopy) {
    int charactersToCopy = defines::Min(copyToSize, numToCopy);
    for (int character = 0; character < charactersToCopy; ++character)
    {
        copyTo[character] = copyFrom[character];
    }
    if (charactersToCopy < copyToSize) {
        copyTo[charactersToCopy] = '\0';
    }
}

inline void PlatformSpecificReverseString(char* buffer) {
    char* start = buffer;
    char* end = buffer;
    while (*end != '\0')
    {
        ++end;
    }
    --end;
    
    for (; start < end; ++start, --end)
    {
        char tmp = *start;
        *start = *end;
        *end = tmp;
    }
}

inline void PlatformSpecificCopyString(char* copyTo, const char* copyFrom, int) {
	strcpy(copyTo, copyFrom);
}
    
inline void PlatformSpecificSleep(unsigned milliseconds) {
    usleep(milliseconds * 1000); // takes microseconds
}

#endif // ifndef _SystemOSX_