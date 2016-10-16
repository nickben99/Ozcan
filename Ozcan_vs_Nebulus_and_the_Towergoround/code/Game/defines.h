//defines.h - useful defines

//system includes------------
#include <math.h> // maths functions
#ifdef _WINDOWS
#include <windows.h>	// for strncpy_s
#include <float.h> // for _isnan
#endif
#include <cmath> // for std::isnan
#ifdef OSX
#include <cstdlib>
#include <string.h>
#endif
//---------------------------

#ifndef _defines_h_
#define _defines_h_

//header files---------------
#include "Game/Random.h"
//---------------------------

#define UNUSED(expr) (void)(expr)

#define EPSILON 1.0e-5   // a very small value 

// floating point equality check, within the bounds of a very small differance
#define FLOAT_EQUALS(N1, N2) (fabs(N1 - (N2)) < EPSILON) ? true : false

#define PI 3.141592654f

// convert degrees to radians
#define degToRad(degrees) degrees*(PI/180.0f)

// convert radians to degrees
#define radToDeg(radians) radians/(PI/180.0f)

/*	returns a float random number between loNumber and highNumber inclusive.
	doesn't matter whether loNumber is higher than highNumber or vice-versa. */
#define FRANDOM(loNumber, highNumber)	(loNumber > highNumber) ? \
						(float)(highNumber+(Random::GetInst().GetRand()%((abs(loNumber-(highNumber)))+1))) :\
						(float)(loNumber+(Random::GetInst().GetRand()%((abs(loNumber-(highNumber)))+1)))

/*	returns a random number between loNumber and highNumber inclusive.
	doesn't matter whether loNumber is higher than highNumber or vice-versa. */
#define RANDOM(loNumber, highNumber)	(loNumber > highNumber) ? \
						(highNumber+(Random::GetInst().GetRand()%((abs(loNumber-(highNumber)))+1))) :\
						(loNumber+(Random::GetInst().GetRand()%((abs(loNumber-(highNumber)))+1)))

const char* GetDirectoryPath();

/*	returns true if a quadratic equation can be solved within defined limits and 
	sets the referance parameter to the lowest root */
bool solveQuadraticEquation(float a, float b, float c, float minRange, float maxRange, 
							float * theRoot);

float ToShaderUIX(float coord);

float ToShaderUIY(float coord);

// make sure acos input is within range
/*	BASED ON METHOD FROM 3D MATH PRIMER FOR GRAPHICS AND GAME DEVELOPMENT 
	- DUNN AND PARBERRY */
inline float safeACos(float x)
{
	if (x > 1.0f){
		return( (float)acos(1.0f) );}
	if (x < -1.0f){
		return( (float)acos(-1.0f) );}
	
	return(	(float)acos(x) );
}

// input should be between 0 and 1
inline float SlowFastSlowLerp(float input)
{
	// ((float)sin((x-0.5)*3.14159)+1)/2 in graphic calculator
	return ((float)sin((input-0.5f)*PI)+1.0f)/2.0f;
}

namespace defines
{

inline float FRandom(const int& loNumber, const int& highNumber)
{
    Random& randInst = Random::GetInst();
    if (loNumber > highNumber)
    {
        return (float)(highNumber+(randInst.GetRand()%((std::abs(loNumber-highNumber))+1)));
    }
    return (float)(loNumber+(randInst.GetRand()%((std::abs(loNumber-highNumber))+1)));
}
    
template<typename T> inline T Min(const T& lhs, const T& rhs)
{
	return (lhs < rhs) ? lhs : rhs;
}

template<typename T> inline T Max(const T& lhs, const T& rhs)
{
	return (lhs > rhs) ? lhs : rhs;
}

template<typename T> inline T Clamp(const T& value, const T& min, const T& max)
{
	return Min(max, Max(value, min));
}

inline bool IsNaN(const float& num)
{
#ifdef _WINDOWS
    return 0 != _isnan(num);
#endif
    
#ifdef OSX
    return std::isnan(num);
#endif
}
    
inline void strncpy(char* copyTo, int copyToSize, const char* copyFrom, int numToCopy)
{
#ifdef _WINDOWS
    strncpy_s(copyTo, copyToSize, copyFrom, numToCopy);
#endif
    
#ifdef OSX
    int charactersToCopy = defines::Min(copyToSize, numToCopy);
    for (int character = 0; character < charactersToCopy; ++character)
    {
        copyTo[character] = copyFrom[character];
    }
#endif
}
    
inline void ReverseString(char* buffer)
{
#ifdef _WINDOWS
    _strrev(buffer); // reverse the latest line of file
#else
    char* start = buffer;
    char* end = buffer;
    while (*end != '\0')
    {
        ++end;
    }
    --end;
    
    for (; start < end; ++start, ++end)
    {
        char tmp = *start;
        *start = *end;
        *end = tmp;
    }
#endif
}

#ifdef _WINDOWS
inline void CopyString(char* copyTo, const char* copyFrom, int copyToSize)
{
	strcpy_s(copyTo, copyToSize, copyFrom);
}
#else
inline void CopyString(char* copyTo, const char* copyFrom, int)
{
	strcpy(copyTo, copyFrom);
}
#endif
    
}

#endif // ifndef _defines_