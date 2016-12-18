// CKeyMap.h - header file for key map class

#ifdef _WINDOWS

#ifndef _CKeyMap_H_
#define _CKeyMap_H_

//--- System Includes -----------
#include <windows.h>
#include <windowsx.h>
//-------------------------------

//--- Header files --------------
#include "Input/CKeyCode.h"
//-------------------------------

//--- defines -------------------
//#define POLLS_PER_SECOND 20 // amount of keyboard polls per second
//-------------------------------

//--- external variables --------
extern HWND			hWnd;			// holds window handle
extern HINSTANCE	hInst;			// holds the instance of the application
//-------------------------------

class ReplayManager;
struct CKeyMapInternal;

class CKeyMap
{
	private:
//----- Private variables ----------
		CKeyMapInternal* internal;
	
		BYTE map[10]; // array of direct input Globals::Instance().keys assigned to functions

		// contains keyboard state table
		UCHAR keyboard_state[256];
		// contains keyboard state table for the previous frame
		UCHAR previousKeyboard_state[256];
		// any Globals::Instance().keys which have changed their state to pressed this frame
		UCHAR changed_state_pressed[256];
		// any Globals::Instance().keys which have changed their state to unpressed this frame
		UCHAR changed_state_released[256];

		//int waitPeriodBetweenPolls,
		//	timeOfLastPoll; // variables for having set times between keyboard polls
//----------------------------------

		// delete direct input
		bool deleteDirectInput();
		BYTE MapToDirectInput(eKeyCode key);

	public:
//----- Public methods ------------------------
		CKeyMap();    // constructor
		~CKeyMap();   // destructor

		bool GetKeyPressed(eKeyCode key);	// see if the direct input key has changed to pressed
		bool GetKeyReleased(eKeyCode key);	// see if the direct input key has changed to unpressed
		bool GetKeyStatus(eKeyCode key);	// see if the direct input key is pressed
  
		// update arrays of which Globals::Instance().keys are pressed
		// and which Globals::Instance().keys have been pressed this cycle
		// updates key input states, returns true if Globals::Instance().keys were updated this frame
		void Update();
		
		// set-up direct input 
		bool Init();
//---------------------------------------------
};

#endif

#endif