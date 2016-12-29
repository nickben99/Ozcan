//Globals.h

//system includes---
//------------------

#ifndef _Globals_h_
#define _Globals_h_

#include "Game/CLevel.h" // holds the current level instance
#include "Input/Interfaces/CKeyMapInterface.h"
#include "Audio/Interfaces/SoundInterface.h"
#include "Time/Interfaces/CTimerInterface.h"
#include "Rendering/Interfaces/OpenGLInterface.h"
#include "Rendering/MatrixStack.h"
#if _DEBUG
#include "Debug/Interfaces/DebugInterface.h"
#include "Debug/DebugRendering.h"
#include "Debug/DebugMenu.h"
#endif

class CMainMenu;

class Globals
{ 
public:
	static Globals& Instance();
	~Globals();

	SoundInterface sound;
	CLevel currentLevel;
	CKeyMapInterface keys;
	CTimerInterface timer;
	MatrixStack modelMatrixStack;
	MatrixStack viewMatrixStack;
#if _DEBUG
	DebugRendering debugDraw;
	DebugMenu debugMenu;
#endif

	float windowWidth;
	float windowHeight;

    // the main menu (this needs to be a pointer so we can forward declare CMainMenu to avoid a circular dependancy)
	CMainMenu* mpMainMenu;
	int gameLevel; // the current level of the game being played

	int (*main)(int);

	int drawMenu; // used to stop the menu being drawn when the game starts
	OpenGLInterface gl;
#if _DEBUG
	DebugInterface debug;
#endif
private:
	Globals();
};

#endif // ifndef _Globals_h_