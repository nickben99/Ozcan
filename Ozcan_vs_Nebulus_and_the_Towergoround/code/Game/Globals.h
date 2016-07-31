//Globals.h

//system includes---
//------------------

#ifndef _Globals_h_
#define _Globals_h_

#ifdef _WINDOWS
#include "Game/CLevel.h" // holds the current level instance
#include "Input/Interfaces/CKeyMapInterface.h"
#include "Audio/Interfaces/SoundInterface.h"
#include "Time/Interfaces/CTimerInterface.h"
#include "Rendering/Interfaces/OpenGLInterface.h"
#include "Rendering\MatrixStack.h"
#if _DEBUG
#include "Debug/Interfaces/DebugInterface.h"
#include "Debug/DebugRendering.h"
#include "Debug/DebugMenu.h"
#endif
#endif // _windows

class CMainMenu;

class Globals
{ 
public:
	static Globals& Instance();
	~Globals();
#ifdef _WINDOWS
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
#endif // _windows
	float windowWidth;
	float windowHeight;

	CMainMenu* mpMainMenu;	// the main menu
	int gameLevel; // the current level of the game being played

	int (*main)(int);

	int drawMenu; // used to stop the menu being drawn when the game starts
#ifdef _WINDOWS
	OpenGLInterface gl;
#if _DEBUG
	DebugInterface debug;
#endif
#endif // _windows
private:
	Globals();
};

#endif // ifndef _Globals_h_