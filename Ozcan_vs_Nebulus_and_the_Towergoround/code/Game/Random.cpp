// Random.cpp
#ifdef _WINDOWS
//--- System includes ---------------
#include <stdlib.h>
//----------------------------------

//--- Header files ---------
#include "Random.h"
#include "ReplayManager.h"
#include "Game/Globals.h"
//--------------------------

Random::Random() // constructor
	: mpReplay(0)
{
	Restart((unsigned)Globals::Instance().timer.time());
}

Random::~Random() //destructor
{
}

Random& Random::GetInst()
{
	static Random randInst;
	return randInst;
}

void Random::Restart(unsigned seed)
{
	srand(seed); // for random number generation
}

int Random::GetRand()
{
	int res = rand();
#ifdef DETERMINISTIC_REPLAY_DEBUG
	if (0 != mpReplay)
	{
		mpReplay->IntResult(res);
	}
#endif
	return res;
}

void Random::SetReplayManager(ReplayManager* pReplay)
{
	mpReplay = pReplay;
}
#endif