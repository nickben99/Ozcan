// CKeyMap.h - header file for key map class

#ifndef _CKeyMap_H_
#define _CKeyMap_H_

//--- System Includes -----------
//-------------------------------

//--- Header files --------------
#include "Input/CKeyCode.h"
//-------------------------------

//--- defines -------------------
//-------------------------------

//--- external variables --------
//-------------------------------

class ReplayManager;
struct CKeyMapInternal;

class CKeyMap
{
	private:
//----- Private variables ----------
		CKeyMapInternal* internal;

		ReplayManager* mpReplay;
//----------------------------------

	public:
//----- Public methods ------------------------
		CKeyMap();    // constructor
		~CKeyMap();   // destructor

		bool GetKeyPressed(eKeyCode key, bool replaySynced = false);	// see if the direct input key has changed to pressed
		bool GetKeyReleased(eKeyCode key);	// see if the direct input key has changed to unpressed
		bool GetKeyStatus(eKeyCode key, bool replaySynced = false);	// see if the direct input key is pressed
  
		void Update();
		
		bool Init();

		void SetReplayManager(ReplayManager* pReplay);
//---------------------------------------------
};

#endif