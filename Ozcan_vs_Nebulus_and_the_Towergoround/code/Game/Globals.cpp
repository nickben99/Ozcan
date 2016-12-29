//Globals.cpp

// ---- header files -----
#include "Globals.h"
#include "Game/CMainMenu.h"
#include "Game/main.h"
// -----------------------

Globals& Globals::Instance()
{
    static Globals theGlobals;
    return theGlobals;
}

Globals::~Globals()
{
    delete mpMainMenu;
}

Globals::Globals()
	: windowWidth(0)
	, windowHeight(0)
	, gameLevel(0)
{
    mpMainMenu = new CMainMenu();
	main = menuSystem;
	drawMenu = 1; // used to stop the menu being drawn when the game starts
}
