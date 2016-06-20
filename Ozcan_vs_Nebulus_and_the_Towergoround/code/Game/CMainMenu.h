/* CMainMenu.h - header for the mainMenu class (includes main menu, menu options as sub 
												classes of CMenuOption) */

//system includes------------
#include <windows.h>		// header file for windows
#include <stdio.h>			// header file for standard input/output
#include <gl\gl.h>			// header file for the OpenGL32 library
#include <gl\glu.h>			// header file for the GLu32 library
//---------------------------

// external variables -------
// --------------------------

// definitions --------------
// --------------------------

#ifndef _CMainMenu_h_
#define _CMainMenu_h_

// header files -------------
#include "CMenu.h" // also includes CMenuOption 
#include "CHUD.h"
#include "Game/defines.h"
// --------------------------

class CExit : public CMenuOption
{
	public:
// public methods ----------------------
		CExit()
		{
			strcpy_s(screenText, "Exit Game");
		}

		int action( void)
		{
			PostQuitMessage(WM_QUIT); // post a quit message to end the game
			return( true ); /*	return type true means the leaveMenu method will 
								not be executed (this stops the curser flicking back 
								to the first option before exiting the game) */
		}
// -------------------------------------
}; // end class CExit

class CLevelOne : public CMenuOption
{
	public:
// public methods ----------------------
		CLevelOne() // constructor
		{
			strcpy_s(screenText, "Play Level 1");
			HUD = CHUD::instance(); // get singleton instance
		}

		int action( void);
// -------------------------------------

// public variables --------------------
		CHUD *HUD;
// -------------------------------------
}; // end class CLevelOne

class CLevelTwo : public CMenuOption
{
	public:
// public methods ----------------------
		CLevelTwo() // constructor
		{
			strcpy_s(screenText, "Play Level 2");
			HUD = CHUD::instance(); // get singleton instance
		}

		int action( void);
// -------------------------------------

// public variables --------------------
		CHUD *HUD;
// -------------------------------------
}; // end class CLevelOne

class CLevelThree : public CMenuOption
{
	public:
// public methods ----------------------
		CLevelThree() // constructor
		{
			strcpy_s(screenText, "Play Level 3");
			HUD = CHUD::instance(); // get singleton instance
		}

		int action( void);
// -------------------------------------

// public variables --------------------
		CHUD *HUD;
// -------------------------------------
}; // end class CLevelOne

class CMainMenu : public CMenu
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables-------------
//---------------------------------

//----public methods---------------
		CMainMenu() // constructor
		{
			CLevelOne *levelOne = new CLevelOne();
			CLevelTwo *levelTwo = new CLevelTwo();
			CLevelThree *levelThree = new CLevelThree();
			CExit	*exit = new CExit();

			menuOptions.push_back(levelOne);
			menuOptions.push_back(levelTwo);
			menuOptions.push_back(levelThree);
			menuOptions.push_back(exit);
		}

		// load background and curser images
		int Load()
		{
			// load the main menus background and curser 
			char background[400];
			sprintf_s(background, "%simages/back2.bmp", GetDirectoryPath());
			char curser[400];
			sprintf_s(curser, "%simages/select.bmp", GetDirectoryPath());
			return( CMenu::Load(background, curser));
		}

		void leaveMenu( void)
		{
			subMenuSelected = // set sub menu selected back to zero
			menuOptionSelected = 0; // set the menu option selected back to zero
		}

		// overloaded operators

//---------------------------------
}; // end class CMainMenu

#endif // ifndef _CMainMenu_h_ 