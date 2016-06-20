//main.h: main game loop functions

#ifndef _main_h_
#define _main_h_

//---system includes---------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

//---header files------------
#include "CSpriteEngine.h"
#include "CLevel.h"
#include "Game/Globals.h"
#include "CNebulus.h"
#include "Camera/CCamera.h"
#include "CMainMenu.h"
//---------------------------

//---defines-----------------
//---------------------------

//---external variables------
extern CSpriteEngine sprites;	// from winSetup
//---------------------------


//---functions---------------
bool initGameMenus(); // initialise the game menus
void deleteGameObjects(); // delete all game objects									
GLint gameMain(GLint level = 1); // main function
GLint menuSystem(GLint a = 0); // the menu system (dummy parameter unnused)
//---------------------------

#endif //_main_h_