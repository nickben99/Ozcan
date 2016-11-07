//Game.cpp, the Game class

// ---- header files -----
#include "Game.h"
#include "Game/GameTime.h"
#include "Game/Globals.h"
#include "Game/main.h"
// -----------------------

Game::~Game()
{
}

Game::Game()
{
}

bool Game::Init()
{
	bool initSucceeded = initGameMenus();
	initSucceeded = initSucceeded && Globals::Instance().sound.Initialize();
	initSucceeded = initSucceeded && Globals::Instance().currentLevel.loadTheHUD();
	initSucceeded = initSucceeded && Globals::Instance().keys.Init();
	initSucceeded = initSucceeded && timer.Init();

	if (!initSucceeded)
	{
		return false; // failure
	}
	
	timer.Start();
	return true;
}

bool Game::Update()
{
	Globals::Instance().keys.Update(); // poll the keyboard

	bool running = 0 != (*Globals::Instance().main)(Globals::Instance().gameLevel); // returns true if ok (syntax can be main(gamelevel) )

	timer.Stop();
	float time = timer.Elapsed();
	time *= 1000.0f; // convert to milliseconds
	/*	get amount to stop for to ensure frame length stays approximatly 60 fps */
	float fixedTimeDelta = static_cast<float>( GameTime::Instance().GetCurrentFixedTimeDeltaMilli() );
	int stopTime = static_cast<int>( ((fixedTimeDelta-time) >= 0.0) ? 16.0f-time : 0.0f );
    
    defines::sleep(stopTime); // keep frame rate around 60 fps
    
	timer.Start();

	return running;
}

void Game::DeleteGameObjects()
{
	deleteGameObjects();
}
