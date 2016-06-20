//Game.h, header file for Game.cpp

#ifndef _Game_h_
#define _Game_h_

//system includes---
//------------------

#include "Time/Interfaces/CHighPrecisionTimerInterface.h"

class Game
{ 
public:	
	Game();
	~Game();

	bool Init();
	bool Update();
	void DeleteGameObjects();

private:
	CHighPrecisionTimerInterface timer;
};

#endif // ifndef _Game_h_