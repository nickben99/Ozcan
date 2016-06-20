//CMainMenu.cpp, the menu class

// system includes --------
//-------------------------

// header files -----------
#include "Game/CMainMenu.h"
#include "Game/Globals.h"
#include "Game/main.h"
//-------------------------

int CLevelOne::action( void)
{
	Globals::Instance().sound.PlaySound( SOUNDS_MENUSELECT, false ); // play menu select sound once

	Globals::Instance().gameLevel = 1; // set the level to be played
	// set function pointer to the main game code
	Globals::Instance().main = gameMain;
	Globals::Instance().drawMenu = 0; // don't draw the menu after the HUD
	HUD->draw(); // will be on intro screen
	HUD->drawIntroScreenLoadingBar( 0.0f ); // draw the bar with 0% loaded

	return( false ); /* return type false indicates the action will result in the 
						current menu screen being exited */
}

int CLevelTwo::action( void)
{
	Globals::Instance().sound.PlaySound( SOUNDS_MENUSELECT, false ); // play menu select sound once

	Globals::Instance().gameLevel = 2; // set the level to be played
	// set function pointer to the main game code
	Globals::Instance().main = gameMain;
	Globals::Instance().drawMenu = 0; // don't draw the menu after the HUD
	HUD->draw(); // will be on intro screen
	HUD->drawIntroScreenLoadingBar( 0.0f ); // draw the bar with 0% loaded

	return( false ); /* return type false indicates the action will result in the 
						current menu screen being exited */
}

int CLevelThree::action( void)
{
	Globals::Instance().sound.PlaySound( SOUNDS_MENUSELECT, false ); // play menu select sound once

	Globals::Instance().gameLevel = 3; // set the level to be played
	// set function pointer to the main game code
	Globals::Instance().main = gameMain;
	Globals::Instance().drawMenu = 0; // don't draw the menu after the HUD
	HUD->draw(); // will be on intro screen
	HUD->drawIntroScreenLoadingBar( 0.0f ); // draw the bar with 0% loaded

	return( false ); /* return type false indicates the action will result in the 
						current menu screen being exited */
}