// CKeyMap.cpp - keymap implementation class

//--- System includes ---------------
#include <stdio.h>
#include <objbase.h>
//----------------------------------

//--- Header files ---------
#include "CKeyMapWindows.h" // the header file for this class
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "Game/ReplayManager.h"

//--------------------------

struct CKeyMapInternal
{
	LPDIRECTINPUT8        lpdi;        // dinput object
	LPDIRECTINPUTDEVICE8  lpdikey;     // dinput keyboard
};

CKeyMap::CKeyMap() // constructor
{
	internal = new CKeyMapInternal();

	internal->lpdi=0;        // set to null
	internal->lpdikey=0;     // set to null

	for (int counter = 0; counter <= 255; counter++){
		previousKeyboard_state[counter] = 0x00;}
}

CKeyMap::~CKeyMap() //destructor
{
	deleteDirectInput();
}

// delete all direct input variables and set them to null
bool CKeyMap::deleteDirectInput()
{
	if (internal->lpdikey)
	{
		internal->lpdikey->Unacquire(); // release keyboard
		internal->lpdikey->Release();
		internal->lpdikey = 0;
	}
	
	if (internal->lpdi)
	{
		internal->lpdi->Release();
		internal->lpdi = 0;
	}

	delete internal;

	return(true);
}

//-----------------------------------------------------------------------------
// DIRECT INPUT KEYBOARD SETUP
//-----------------------------------------------------------------------------

bool CKeyMap::Init()
{
	// initialise direct input keyboard device
	// first create the direct input object
	#if 0  // directinput7 method
		// first create the direct input object
		if (FAILED(DirectInputCreateEx(hInst, DIRECTINPUT_VERSION,
			IID_IDirectInput7, (void **)&internal->lpdi,0))){
			return FALSE;}
		// create a keyboard device
		if (FAILED(internal->lpdi->CreateDeviceEx(GUID_SysKeyboard, IID_IDirectInputDevice7,
			(void **)&internal->lpdikey, 0))){
			return FALSE;}
	#endif
	
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void **) &internal->lpdi, 0))){
		return (0);}
  
	// create a keyboard device
	if (FAILED(internal->lpdi->CreateDevice(GUID_SysKeyboard, &internal->lpdikey, 0))){
		return (0);}

	// set cooperation level
	if (FAILED(internal->lpdikey->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE |
		DISCL_BACKGROUND))){
		return (0);}

	// set data format
	if (FAILED(internal->lpdikey->SetDataFormat(&c_dfDIKeyboard))){
		return (0);}

	// acquire the keyboard
	if (FAILED(internal->lpdikey->Acquire())){
		return (0);}

  return (1); // success
}

// updates key input states, returns true if Globals::Instance().keys were updated this frame
void CKeyMap::Update()
{
	// if wait period has elapsed, poll the keyboard
//	if ( timer.elapsed(timeOfLastPoll, waitPeriodBetweenPolls) )
	{
		// get state of all the Globals::Instance().keys and put into the keyboard_state array
		internal->lpdikey->GetDeviceState(256, (LPVOID)keyboard_state);

		for (int counter = 0; counter <= 255; ++counter)
		{
			// for all keyboard Globals::Instance().keys, if they were not pressed in the previous cycle
			// and they are pressed now set the appropriate position in the
			// changed_state_pressed array to true
			if ((!(previousKeyboard_state[counter] &0x80)) &&
				(keyboard_state[counter] &0x80)){
				changed_state_pressed[counter] = 1;}
			else{
				changed_state_pressed[counter] = 0;}

			// for all keyboard Globals::Instance().keys, if they were pressed in the previous cycle
			// and they are not pressed now set the appropriate position in the
			// changed_state_unpressed array to true
			if ((previousKeyboard_state[counter] &0x80) && 
				(!(keyboard_state[counter] &0x80))){
				changed_state_released[counter] = 1;}
			else{
				changed_state_released[counter] = 0;}

			// set the previousKeyboard_state array elements to the keyboard_state
			// array
			previousKeyboard_state[counter] = keyboard_state[counter];
		}// end for counter
	}// end if timer wait period has elapsed
	//return(false); // indicates key states were not updated this frame
}

bool CKeyMap::GetKeyPressed(eKeyCode key)
{	
	BYTE DIKey = MapToDirectInput(key);	
	// see if the input key sent as a parameter
	// was not pressed in the previous cycle and
	// is now pressed in this cycle
	return 1 == changed_state_pressed[DIKey] ? true : false;
}

bool CKeyMap::GetKeyReleased(eKeyCode key)
{
	BYTE DIKey = MapToDirectInput(key);
	// see if the input key sent as a parameter
	// was pressed in the previous cycle and
	// is now not pressed in this cycle
	return 1 == changed_state_released[DIKey] ? true : false;
}

bool CKeyMap::GetKeyStatus(eKeyCode key)
{
	BYTE DIKey = MapToDirectInput(key);
	// see if the key sent as a parameter
	// has been pressed this cycle
	return keyboard_state[DIKey] ? true : false;
}

BYTE CKeyMap::MapToDirectInput(eKeyCode key)
{
	switch (key)
	{
	case eKeyCode_RETURN: return DIK_RETURN;
	case eKeyCode_C: return DIK_C;
	case eKeyCode_LSHIFT: return DIK_LSHIFT; 
	case eKeyCode_RSHIFT: return DIK_RSHIFT;
	case eKeyCode_P: return DIK_P;
	case eKeyCode_J: return DIK_J;
	case eKeyCode_L: return DIK_L;
	case eKeyCode_I: return DIK_I;
	case eKeyCode_K: return DIK_K;
	case eKeyCode_R: return DIK_R;
	case eKeyCode_T: return DIK_T;
	case eKeyCode_F: return DIK_F;
	case eKeyCode_A: return DIK_A;
	case eKeyCode_D: return DIK_D;
	case eKeyCode_E: return DIK_E;
	case eKeyCode_Q: return DIK_Q;
	case eKeyCode_W: return DIK_W;
	case eKeyCode_S: return DIK_S;
	case eKeyCode_N: return DIK_N;
	case eKeyCode_TAB: return DIK_TAB;
	case eKeyCode_UP: return DIK_UP;
	case eKeyCode_DOWN: return DIK_DOWN;
	case eKeyCode_LEFT: return DIK_LEFT;
	case eKeyCode_RIGHT: return DIK_RIGHT;
	case eKeyCode_SPACE: return DIK_SPACE;
	case eKeyCode_M: return DIK_M;
	case eKeyCode_NUMPAD0: return DIK_NUMPAD0;
	case eKeyCode_NUMPAD1: return DIK_NUMPAD1;
	case eKeyCode_NUMPAD2: return DIK_NUMPAD2;
	case eKeyCode_NUMPAD3: return DIK_NUMPAD3;
	case eKeyCode_NUMPAD4: return DIK_NUMPAD4;
	case eKeyCode_NUMPAD5: return DIK_NUMPAD5;
	case eKeyCode_1: return DIK_1;
	case eKeyCode_2: return DIK_2;
	}
	return 0;
}