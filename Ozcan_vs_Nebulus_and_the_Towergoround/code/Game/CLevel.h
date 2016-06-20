//CLevel.h - header for the level class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
#include <stdio.h>		// for sprintf_s
//---------------------------

#ifndef _CLevel_h_
#define _CLevel_h_

//header files---------
#include "FileReading/CTextFileReader.h" // for reading files
#include "CScene.h" // holds the tower structure information
#include "CSpriteEngine.h"
#include "COctree.h" // the level octree
#include "CHUD.h" // the head up display
#include "CNebulus.h"
#include "Rendering/CParticleSystem.h" // the particle system
#include "ReplayManager.h"
#include "Game/GameTime.h"
//---------------------

//external variables---
extern HDC hDC; // from winSetup
//---------------------

//defines--------------
//---------------------

class CLevel
{
	private:
//-----private variables-----------
		GLint	level; // which level this is
		GameTime& mGameTime;	
		bool levelInProgress; // whether the level is currently in progress		
		ReplayManager* replay;
		int mFrameCounter;
		bool startWholeLevelReplay;

private:
	CLevel(const CLevel& copy)
		: mGameTime(copy.mGameTime)
		, startWholeLevelReplay(false)
	{}
	CLevel& operator=(const CLevel&){return *this;};
	
//---------------------------------

	public:
//----public variables--------------
		CScene theScene; // the scene object	
		COctree levelOctree; // the levels octree
		CSpriteEngine theSprites; // the level octree
		bool mIsLoading;
		CHUD *theHUD; // the level HUD
		CNebulus *theNebulus; // pointer to the Nebulus
		GLuint	totalLevelTime, // the time allowed for this level (from level file) (milliseconds)
				levelTimer; // level time elapsed
		int		levelTimeRemaining; // based on time subtracted from 'totalLevelTime' above
//----------------------------------

//----public methods----------------------------------------
		CLevel(); // constructor
		~CLevel(); // destructor
		bool loadLevel(GLint theLevel); /* load this levels text description file and 
										initialise appropriate objects */
		// load the HUD graphics
		int loadTheHUD();
		void deleteCurrentLevel(); // delete currently loaded level
		// start the level again
		void reStartLevel(ReplayManager::eReplayStyle style);
		void processKeyInput(); // perform actions on sprites based on key input
		void move(); // moves the objects in the level
		void collisionDetection(); // look for collisions
		void draw(); // draws the objects in the level
		void deleteLevel(); // delete all objects initialised for this level
		void updateTime(); // update the level timer
		void updateReplay();
		void CheckStartWholeLevelReplay();

		// inline methods
		// returns whether the level is currently in progress
		inline bool islevelInProgress() {return(levelInProgress);}
		inline int GetFrameNumber() {return mFrameCounter; }

	private:
		void Draw3DScene();
//-----------------------------------------------------------
}; // end class CLevel

#endif // ifndef _CLevel_h_ 