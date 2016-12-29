//CExitScreen.cpp, an exit screen class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CExitScreen.h"
//-------------------------

//constructor
CExitScreen::CExitScreen()
{
}

// overloaded constructor
CExitScreen::CExitScreen(CModel * aModel, int towerPosition)
	: CTowerSprite(aModel, towerPosition) // initialise super class
{
	initialise(); // initialisation
}	

// initialise class
void CExitScreen::initialise()
{	
	spriteType = SEXIT_SCREEN; // set sprite type
}

// destructor
CExitScreen::~CExitScreen()
{
}
