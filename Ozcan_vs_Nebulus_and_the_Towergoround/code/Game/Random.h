// Random.h

#ifndef _Random_H_
#define _Random_H_

//--- System Includes -----------
//-------------------------------

//--- Header files --------------
//-------------------------------

//--- defines -------------------
//-------------------------------

class ReplayManager;

//--- external variables --------
//-------------------------------

class Random
{
	private:
		ReplayManager* mpReplay;

	public:
		Random();
		~Random();

		static Random& GetInst();

		void Restart(unsigned seed);
		int GetRand();

		void SetReplayManager(ReplayManager* pReplay);
};

#endif
