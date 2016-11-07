//main.h: main game loop functions

#ifndef _main_h_
#define _main_h_
//---system includes---------
#include <Rendering/OpenGLInclude.h>
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


//---functions---------------
bool initGameMenus(); // initialise the game menus
void deleteGameObjects(); // delete all game objects									
int gameMain(int level = 1); // main function
int menuSystem(int a = 0); // the menu system (dummy parameter unnused)
//---------------------------

#endif //_main_h_