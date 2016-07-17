//main.h: main game loop functions

#ifndef _main_h_
#define _main_h_
//---system includes---------
#include "Rendering/OpenGLInclude.h"
//---------------------------

//---header files------------
#include "CSpriteEngine.h"
#ifdef _WINDOWS
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
#endif
#endif //_main_h_