//SystemOSX.h - useful defines

#ifndef _SystemOSX_h_
#define _SystemOSX_h_

#include <stdarg.h> // for va_list variable length parameter list
#include <stdio.h> // for vsprintf_s
#include <cmath> // for std::isnan
#include <cstdlib>
#include <string.h>
#include <unistd.h> // for usleep
#include <sys/stat.h> // for POSIX stat
#include <iostream> // for cout

#define SPRINTF sprintf

#define VSPRINTF vsprintf

#define SSCANF sscanf

#define SSCANF_STR_PARAM(theStringParamPointer, theStringLength) theStringParamPointer

#define FOPEN(filePointer, fileName, readWriteMode) filePointer = fopen(fileName, readWriteMode);

#define STRCAT(targetBuffer, targetBufferLength, fromBuffer) strcat(targetBuffer, fromBuffer);

inline void ErrorOutput(const char*) {

}

inline void ToConsole(const char* debugString, ...) {

    char thisString[2048];
    va_list vlist; // va_list

    va_start(vlist, debugString); // set to beginning of argument list
    VSPRINTF(thisString, debugString, vlist); // print formatted string to thisString
    va_end(vlist); // reset argument pointer
            
    std::cout << thisString;
}

inline bool PlatformSpecificIsNaN(const float& num) {
    return std::isnan(num) || std::isinf(num);
}

inline void PlatformSpecificStrncpy(char* copyTo, int copyToSize, const char* copyFrom, int numToCopy) {
    int charactersToCopy = (copyToSize < numToCopy) ? copyToSize : numToCopy;
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

inline bool PlatformSpecificDoesDirExists(const char* dirName)
{
    if ((nullptr != dirName) && (access(dirName, 0) == 0))
    {
		struct stat status;
		stat( dirName, &status );
		if( status.st_mode & S_IFDIR ) {
			return true;
		}
    }
    // if any condition fails
    return false;
}

#endif // ifndef _SystemOSX_