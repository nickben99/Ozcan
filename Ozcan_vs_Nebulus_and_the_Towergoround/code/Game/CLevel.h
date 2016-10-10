//CLevel.h - header for the level class

//system includes------------
#include "Rendering/OpenGLInclude.h"
#include <stdio.h>		// for sprintf_s
//---------------------------

#ifndef _CLevel_h_
#define _CLevel_h_
#ifdef _WINDOWS
//header files---------
#include "FileReading/CTextFileReader.h" // for reading files
#endif // _WINDOWS
#include "CScene.h" // holds the tower structure information
#ifdef _WINDOWS
#include "CSpriteEngine.h"
#endif
#include "COctree.h" // the level octree
#ifdef _WINDOWS
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
#endif // _WINDOWS
class CLevel
{
#ifdef _WINDOWS
	private:
//-----private variables-----------
		GLint	level; // which level this is
		GameTime& mGameTime;	
		bool levelInProgress; // whether the level is currently in progress
		ReplayManager* replay;
#endif // _WINDOWS
		int mFrameCounter;
		bool startWholeLevelReplay;
#ifdef _WINDOWS
private:
	CLevel(const CLevel& copy)
		: mGameTime(copy.mGameTime)
		, startWholeLevelReplay(false)
	{}
	CLevel& operator=(const CLevel&){return *this;};
	
//---------------------------------
#endif
	public:
//----public variables--------------
		CScene theScene; // the scene object
		COctree levelOctree; // the levels octree
#ifdef _WINDOWS
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
#endif //_WINDOWS
		inline int GetFrameNumber() {return mFrameCounter; }
#ifdef _WINDOWS
	private:
		void Draw3DScene();
#endif // _WINDOWS
//-----------------------------------------------------------
}; // end class CLevel

#endif // ifndef _CLevel_h_