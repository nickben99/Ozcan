// defines.cpp - useful (mainly mathematical) definitions and methods

//--- System includes -----------------
//-------------------------------------

//--- Header files --------------------
#include "Game/defines.h" // the header file
#include "FileReading\CTextFileReader.h"
#include "Game\Globals.h"
//-------------------------------------

//global variables---------------------
//-------------------------------------

const char * const pEmptyPath = "";
const char * const pSubPath = "Ozcan_vs_Nebulus_and_the_Towergoround/";

const char* GetDirectoryPath()
{
	if (CTextFileReader::DoesDirExists(pSubPath))
	{
		return pSubPath;
	}
	return pEmptyPath;
}

/*	returns true if a quadratic equation can be solved within defined limits and 
	sets the referance parameter to the lowest root */
bool solveQuadraticEquation(float a, float b, float c, float minRange, float maxRange, 
							float * theRoot)
{
	// check if radicand is negative as their is no solution to a negative radicand
	float radicand = (b*b) - 4.0f*a*c;

	if (radicand < 0.0f){
		return( false );} // no solution

	// calculate the two roots (if radicand is equal to zero, the roots will be equal)
	float	sqrtRad = (float)sqrt(radicand),
			root1 = ((-b) - sqrtRad) / (2*a),
			root2 = ((-b) + sqrtRad) / (2*a);

	// make sure root one is lower than root two
	if (root1 > root2)
	{	// swap the root around
		float temp = root2;
		root2 = root1;
		root1 = temp;
	}// end if root1 > root2

	// if root one is within the valid range
	if (root1 > minRange && root1 < maxRange)
	{
		*theRoot = root1; // set lowest root
		return( true ); // true - root found
	}

	// if root one was out of range check root two
	if (root2 > minRange && root2 < maxRange)
	{
		*theRoot = root2; // set lowest root
		return( true ); // true - root found
	}

	// no roots were found in the required range
	return( false );
}

float ToShaderUIX(float coord)
{
	float conversion = (Globals::Instance().windowWidth/2.0f);
	return coord-conversion;
}

float ToShaderUIY(float coord)
{
	float conversion = (Globals::Instance().windowHeight/2.0f);
	return coord-conversion;
}