// CKeyMap.h - header file for key map class

#ifndef _CKeyMap_H_
#define _CKeyMap_H_

//--- System Includes -----------
#include </usr/local/Cellar/glfw3/3.1.2/include/GLFW/glfw3.h>
//-------------------------------

//--- Header files --------------
#include "Input/CKeyCode.h"
//-------------------------------

//--- defines -------------------
//-------------------------------

//--- external variables --------
extern GLFWwindow* osxWindow;
//-------------------------------

class ReplayManager;
struct CKeyMapInternal;

class CKeyMap
{
	private:
        // contains keyboard state table
        bool keyboard_state[eKeyCode_Max];
        // contains keyboard state table for the previous frame
        bool previousKeyboard_state[eKeyCode_Max];
        // any Globals::Instance().keys which have changed their state to pressed this frame
        bool changed_state_pressed[eKeyCode_Max];
        // any Globals::Instance().keys which have changed their state to unpressed this frame
        bool changed_state_released[eKeyCode_Max];
    
        int MapToGLFW(eKeyCode key);

	public:
//----- Public methods ------------------------
		CKeyMap();    // constructor
		~CKeyMap();   // destructor

		bool GetKeyPressed(eKeyCode key);	// see if the direct input key has changed to pressed
		bool GetKeyReleased(eKeyCode key);	// see if the direct input key has changed to unpressed
		bool GetKeyStatus(eKeyCode key);	// see if the direct input key is pressed
  
		void Update();
		
		bool Init();
//---------------------------------------------
};

#endif