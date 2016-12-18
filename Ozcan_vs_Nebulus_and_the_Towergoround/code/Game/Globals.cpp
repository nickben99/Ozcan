//Globals.cpp

// ---- header files -----
#include "Globals.h"
#include "Game/CMainMenu.h"
#include "Game/main.h"
// -----------------------

Globals* Globals::inst = nullptr;

Globals& Globals::Instance()
{
    if (!inst) {
        inst = new Globals();
    }
	return *inst;
}

void Globals::Destroy()
{
    if (inst) {
        delete inst;
        inst = nullptr;
    }
}

Globals::~Globals()
{
	delete mpMainMenu;
    delete debugMenu;
}

Globals::Globals()
	: windowWidth(0)
	, windowHeight(0)
	, gameLevel(0)
{
    debugMenu = new DebugMenu();
	mpMainMenu = new CMainMenu();
	main = menuSystem;
	drawMenu = 1; // used to stop the menu being drawn when the game starts
}
