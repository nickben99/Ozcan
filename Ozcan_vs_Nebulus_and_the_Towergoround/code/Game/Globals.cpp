//Globals.cpp

// ---- header files -----
#include "Globals.h"
#ifdef _WINDOWS
#include "Game/CMainMenu.h"
#include "Game/main.h"
#endif
// -----------------------

Globals& Globals::Instance()
{
	static Globals theGlobals;
	return theGlobals;
}

Globals::~Globals()
{
#ifdef _WINDOWS
	delete mpMainMenu;
#endif
}

Globals::Globals()
	: windowWidth(0)
	, windowHeight(0)
	, gameLevel(0)
{
#ifdef _WINDOWS
	mpMainMenu = new CMainMenu();
	main = menuSystem;
	drawMenu = 1; // used to stop the menu being drawn when the game starts
#endif
}
