// CKeyMap.cpp - keymap implementation class

#ifndef _WINDOWS

//--- System includes ---------------
//----------------------------------

//--- Header files ---------
#include "CKeyMapiOS.h" // the header file for this class
#define DIRECTINPUT_VERSION 0x0800
#include "Game/ReplayManager.h"
#include "Debug/debug.h"
//--------------------------

struct CKeyMapInternal
{
};

CKeyMap::CKeyMap() // constructor
{
	internal = new CKeyMapInternal();

	mpReplay = NULL;
}

CKeyMap::~CKeyMap() //destructor
{
}


void CKeyMap::SetReplayManager(ReplayManager* pReplay)
{
	mpReplay = pReplay;
}

bool CKeyMap::Init()
{
	return true; // success
}

void CKeyMap::Update()
{
}

bool CKeyMap::GetKeyPressed(eKeyCode, bool)
{	
	return true;
}

bool CKeyMap::GetKeyReleased(eKeyCode)
{
	return true;
}

bool CKeyMap::GetKeyStatus(eKeyCode, bool)
{
	return true;
}

#endif