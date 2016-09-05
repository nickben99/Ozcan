// CKeyMap.cpp - keymap implementation class

#ifdef OSX

//--- System includes ---------------
//----------------------------------

//--- Header files ---------
#include "CKeyMapOSX.h" // the header file for this class
#include "Game/ReplayManager.h"
//--------------------------

CKeyMap::CKeyMap() // constructor
{
	mpReplay = nullptr;
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