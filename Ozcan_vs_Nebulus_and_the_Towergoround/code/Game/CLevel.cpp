//CLevel.cpp, the level class

// system includes --------
#include <assert.h>
#include <stdio.h>
#include <string.h>
//-------------------------

// header files -----------
#include "CLevel.h"

#include "Game/defines.h"
#include "CMenu.h"
#include "Game/Main.h"
#include "Rendering/ShadowMapping.h"
#include <System/Interfaces/SystemInterface.h>
//-------------------------

// constructor
CLevel::CLevel()
	: mGameTime(GameTime::Instance())
	, startWholeLevelReplay(false)
{
	level = 0;
	levelInProgress = false; // level currently not in progress
	mGameTime.Pause(true); // paused = 1; // game starts in a paused state

	levelTimer = 0; // record the length of the level
	levelTimeRemaining = 0; // total time to complete level

	replay = ReplayManager::instance();

	mIsLoading = false;
	theScene.theSprites = &theSprites;
}

// destructor
CLevel::~CLevel()
{
	deleteCurrentLevel();
}

// delete the currently loaded level
void CLevel::deleteCurrentLevel()
{
	theScene.deleteCurrentScene();
	levelOctree.destroyOctree(); // destroy the octree
	theSprites.deleteCurrentModelsSprites();

	mGameTime.Reset();
	mGameTime.Pause(true); //paused = 1; // game starts in a paused state

	levelTimer = 0; // record the length of the level
	levelTimeRemaining = 0; // total time to complete level

#ifdef _DEBUG
	Globals::Instance().debugDraw.DestroyShapes();
#endif
	ShadowMapping::Destroy();
}

void DrawIntroScreenLoadingBar(CHUD* theHUD, float completion)
{
	theHUD->draw();
	theHUD->drawIntroScreenLoadingBar( completion );
#ifdef _WINDOWS
	SwapBuffers( hDC ); // swap the frame buffers
#else
    glfwSwapBuffers(osxWindow);
#endif
}

#if (_DEBUG && USE_SHADERS)
bool drawShadowMap = false;
bool variablesAddedToDebugMenu = false;
#endif

bool CLevel::loadLevel(GLint theLevel)
{
#if (_DEBUG && USE_SHADERS)
	Globals::Instance().debugDraw.InitShapes();
	if (!variablesAddedToDebugMenu)
	{
		variablesAddedToDebugMenu = true;
		Globals::Instance().debugMenu.AddVariable(new DebugMenu::BoolDebugMenuItem("drawShadowMap", &drawShadowMap));
	}
#endif

	if (!ShadowMapping::Create())
	{
        ErrorOutput("Could not create depth texture");
		return false;
	}

	mIsLoading = true;
	startWholeLevelReplay = false;

	mGameTime.Pause(true); //paused = 1;
	CPowerUpManager::instance()->removeAllPowerUpsFromList();

	levelInProgress = true; // level now in progress
	level = theLevel; // set the level number

	CTextFileReader fileReader; // for file reading
	char buffer[256]; // char buffer holds the level descriptor filename

    SPRINTF(buffer, "%slevels/level%d.txt", GetDirectoryPath(), level); // create file name with path

	if (!fileReader.openFile(buffer, ios::in)){
		ErrorOutput("level file could not be opened");
		return(false);} // return false if file could not be opened

	DrawIntroScreenLoadingBar(theHUD, 0.1f);

	fileReader.getNextLine(); /*	returns the next line which is not blank
									or a comment line */

	// get the the time this level runs for
    SSCANF(fileReader.currentLineOfFile, "%d", &totalLevelTime);
	totalLevelTime*=1000; // convert level time from seconds to milliseconds
	fileReader.getNextLine();

	const int kBackgroundTextLen = 256;
	char tempBackgroundTexturePath[kBackgroundTextLen];
	/*	get the background texture for this level (get everything between 
		the two quotation marks in the file) */
	SSCANF(fileReader.currentLineOfFile, "\"%[^\"]", SSCANF_STR_PARAM(tempBackgroundTexturePath, kBackgroundTextLen));
	
	char backgroundTexture[kBackgroundTextLen];
    SPRINTF(backgroundTexture, "%s%s", GetDirectoryPath(), tempBackgroundTexturePath);
		
	fileReader.getNextLine();
	
	int totalRows, totalColumns, rowHeight, columnWidth;

	// get the number of rows for the scene's tower
    SSCANF(fileReader.currentLineOfFile, "%d %d",	&totalRows,
													&rowHeight);
	fileReader.getNextLine();

	// get the number of columns for the scene's tower
    SSCANF(fileReader.currentLineOfFile, "%d %d",	&totalColumns,
													&columnWidth);
	fileReader.getNextLine();

	if(!theScene.tower->initialise(totalRows, totalColumns, rowHeight, columnWidth)){
		return(false);}

	DrawIntroScreenLoadingBar(theHUD, 0.2f);

	/*	go through all the rows and columns getting the objects in each position.
		dynamic array is filled in reverse order of rows as the first row encountered
		in the text file is the top row of the tower */
	for(int row = totalRows-1; row > -1; row--)
	{
		for(int col = 0; col < totalColumns; col++)
		{	// put char object indicator in correct position
			theScene.tower->towerObjects[(row*totalColumns)+col].objectType =
								fileReader.currentLineOfFile[col];
		}// end for columns
		fileReader.getNextLine();
	}// end for rows

	DrawIntroScreenLoadingBar(theHUD, 0.3f);

	int numberOfLegendItemsToLoad,
		*legendInformation = 0;

	// get the number of legend information items of sprites associated with models
    SSCANF(fileReader.currentLineOfFile, "%d", &numberOfLegendItemsToLoad);
	fileReader.getNextLine();

	/*	allocate memory for legend information 
		(multiplication by two is because each legend item is described
		by two integers (spriteID and modelID) */
	legendInformation = new int[numberOfLegendItemsToLoad*2];
	/*	NOTE - the above memory allocated is deleted in the sprite engine class 
		which receives a pointer to this information later in this method */

	if (!legendInformation){
		return(false);} // memory allocation failed

	{
		char temp; // used to get char from file before casting as int
		int tempInt = 0;
		// put legend information into dynamic array
		for (int legendCounter = 0; legendCounter < numberOfLegendItemsToLoad;
			legendCounter++)
		{
			// get spriteID along with corresponding modelID
			SSCANF(fileReader.currentLineOfFile, "%c %d", SSCANF_STR_PARAM(&temp, 1), &tempInt);
            
			legendInformation[(legendCounter * 2) + 1] = tempInt;
			legendInformation[legendCounter * 2] = (int)temp; // cast to int
			fileReader.getNextLine();
		}
	}

	DrawIntroScreenLoadingBar(theHUD, 0.4f);

	int tempNoOfModelsToLoad; // number of sprite models to load
	char** tempModelFileNames = 0; //file names

	// get the number of models which require loading for this level
    SSCANF(fileReader.currentLineOfFile, "%d", &tempNoOfModelsToLoad);
	fileReader.getNextLine();

	// allocate memory for the array of file names sent to the sprite engine
	tempModelFileNames = new char*[tempNoOfModelsToLoad];

	if (!tempModelFileNames){
		return(false);} // memory allocation failed

	const int kStringLen = 200;
	char path[kStringLen], tempFilename[kStringLen], tempPath[kStringLen];
	// set the path to this levels models
    SPRINTF(path, "%s%d%s", "Level", level, "/");
    defines::CopyString(tempFilename, "", kStringLen); // set up temporary variables
	defines::CopyString(tempPath, path, kStringLen);
	
	// put all model file names into dynamic array
	for (int modelFilename = 0; modelFilename < tempNoOfModelsToLoad; modelFilename++)
	{
		tempModelFileNames[modelFilename] = new char[kStringLen]; // allocate memory
		 // get filename
		SSCANF(fileReader.currentLineOfFile, "%s", SSCANF_STR_PARAM(tempFilename, kStringLen));

		// append filename to path
        STRCAT(tempPath, kStringLen, tempFilename);
		/*	copy tempPath (which contains the filename as well, from above commenad) 
			to tempModelFileNames[modelFilename] */
        defines::CopyString(tempModelFileNames[modelFilename], tempPath, kStringLen);

		defines::CopyString(tempFilename, "", kStringLen); // reset tempFilename and tempPath
		defines::CopyString(tempPath, path, kStringLen);

		fileReader.getNextLine();
	}

	DrawIntroScreenLoadingBar(theHUD, 0.5f);

	// load this levels models
	if (!theSprites.loadModels(&tempModelFileNames[0], tempNoOfModelsToLoad)){
        ErrorOutput("model loading failed");
        delete[] legendInformation;
        legendInformation = 0;
        
		return(false);} // model loading failed

	DrawIntroScreenLoadingBar(theHUD, 0.6f);

	// delete the temporary array of file name strings
	for (int modelFilename = 0; modelFilename < tempNoOfModelsToLoad; modelFilename++)
	{
		if (tempModelFileNames[modelFilename])
		{
			delete[] tempModelFileNames[modelFilename]; // delete string
			tempModelFileNames[modelFilename] = 0;
		}
	}
	// reclaim memory
	if (tempModelFileNames)
	{
		delete[]tempModelFileNames;
		tempModelFileNames = 0;
	}

	int tempNumberOfNonTowerSpritesToLoad;
	int *nonTowerSpriteInformation = 0;
		
	// get number of non tower sprites required for this level
    SSCANF(fileReader.currentLineOfFile, "%d", &tempNumberOfNonTowerSpritesToLoad);
	fileReader.getNextLine();

	/*	allocate memory (multiplied by two as each non tower sprite has a number 
		indicating how many of them their are */
	nonTowerSpriteInformation = new int[tempNumberOfNonTowerSpritesToLoad*2];

	if (!nonTowerSpriteInformation){
		return(false);} // memory allocation failed
	
	// get all the sprite information
	for (int sprite = 0; sprite < tempNumberOfNonTowerSpritesToLoad; sprite++)
	{
		char temp; // used to get char from file before casting as int
		SSCANF(fileReader.currentLineOfFile, "%c %d", SSCANF_STR_PARAM(&temp, 1), &nonTowerSpriteInformation[(sprite*2)+1]);
		nonTowerSpriteInformation[sprite*2] = (int)temp; // cast to int
		fileReader.getNextLine();
	}

	/*	temp array of tower object chars used to send to the sprite engine
		so it can parse the tower object chars to see how many sprites to load */
	char * tempTowerObjects = new char[totalRows*totalColumns];
	
	// copy tower object chars to temporary char array
	for (int i = 0; i < totalRows*totalColumns; i++){
		tempTowerObjects[i] = theScene.tower->towerObjects[i].objectType;}

	DrawIntroScreenLoadingBar(theHUD, 0.7f);

	// load this levels sprites
	if (!theSprites.loadSprites(&nonTowerSpriteInformation[0], 
								tempNumberOfNonTowerSpritesToLoad,
								&tempTowerObjects[0],
								totalRows*totalColumns,
								legendInformation,
								numberOfLegendItemsToLoad)){
		return(false);} // sprite initiation failed
		
	DrawIntroScreenLoadingBar(theHUD, 0.8f);

	// delete temporary array of tower objects
	if (tempTowerObjects)
	{
		delete[] tempTowerObjects; 
		tempTowerObjects = 0;
	}

	// delete the temporary array of sprite information
	if (nonTowerSpriteInformation)
	{
		delete[]nonTowerSpriteInformation;
		nonTowerSpriteInformation = 0;
	}
	
	fileReader.closeFile();

	if (!theScene.initialise(backgroundTexture)){ // do any initialisation in the scene
        ErrorOutput(backgroundTexture);
		return(false);} // return false if scene initialisation problems

	levelOctree.getSceneDimensionsAndTriangles(&theSprites.sprites[0], 
										theSprites.numberOfSprites); // set-up Octree scene

	DrawIntroScreenLoadingBar(theHUD, 0.9f);

	levelOctree.createNodeAndSubNodes(); // create the octree

	theSprites.levelOctree	= &levelOctree; // give sprite engine octree pointer

	theHUD->theNebulus = CNebulus::getExistingInstance(); // set HUD nebulus pointer
	theHUD->levelTimer = &levelTimeRemaining;

	theNebulus = CNebulus::getExistingInstance(); // get Nebulus instance

	// load the particle system texture etc
	if (!CParticleSystem::initialiseParticleSystem()){
		return( false );}

	mIsLoading = false;

	mGameTime.Reset();
	
	replay->StartRecording();
	mFrameCounter = 0;
	replay->IncrementFrame();
	Random::GetInst().SetReplayManager(replay);
	theSprites.setMovingSpritesBack();
	theSprites.saveReplayUpdate(mFrameCounter); // must be called here as updateReplay willnot do anything whilst paused

	theScene.theCamera->initialise();

	Globals::Instance().keys.SetReplayManager(replay);

	DrawIntroScreenLoadingBar(theHUD, 1.0f);

	return(true); // successfully loaded level
}

// load the HUD graphics
int CLevel::loadTheHUD()
{
	theHUD = CHUD::instance(); // get singleton HUD instance

	if (!theHUD){
		return( 0 );} // return false if null

	if (!theHUD->loadHUDGraphicsAndFont(Globals::Instance().windowWidth, Globals::Instance().windowHeight)){ // load the hud graphics and font
		return( 0 );} // return false
	
	return( 1 );
}

// update level clock
void CLevel::updateTime()
{
	mGameTime.Update();
	if (!mGameTime.IsPaused())
	{
		levelTimer += mGameTime.GetTimeDeltaMilli(); // keep track of how much of the level has elapsed
		levelTimeRemaining = totalLevelTime - levelTimer; // subtract time from level time	
	}	
}

// start the level again
void CLevel::reStartLevel(ReplayManager::eReplayStyle style)
{
	mGameTime.Reset();

	switch (style)
	{
	case ReplayManager::eRSWholeLevel:
	case ReplayManager::eRSEnd:
		replay->StartReplaying(style);		

		theHUD->currentHUDState = REPLAY; // restart level straight away
		mGameTime.Pause(false); //paused = 0; // the game should not be paused
		
		break;
	case ReplayManager::eRSMax:
		replay->StartRecording();

		// set HUD to reinitialise screen
		theHUD->currentHUDState = RESTART_SCREEN; // restart level screen
		// pause the game
		mGameTime.Pause(true); //paused = 1; // pause the game

		break;
	default: break;
	}

	mFrameCounter = 0;
	replay->IncrementFrame();	
	theSprites.setMovingSpritesBack();
	theSprites.saveReplayUpdate(mFrameCounter); // must be called here as updateReplay will not do anything whilst paused
	
	/*	reinitialise the camera so all camera velocity is removed
		and the camera is as it was at the beginning */
	theScene.theCamera->initialise();

	if (replay->IsReplayingEndLevel())
	{
		theScene.theCamera->setCameraMode(STATIC_REPLAY);
		theScene.theCamera->mPriorityTypes.Add(CameraPriorityTypes::CamTypeAndData(CamType::eCIReplay, 2));
	}

	levelTimer = 0; // level has not yet begun
	levelTimeRemaining = totalLevelTime;

	startWholeLevelReplay = false;
}

// perform action on sprites based on key input
void CLevel::processKeyInput()
{
	mGameTime.ProcessKeyInput();
	// switch on the HUD state
	switch(theHUD->currentHUDState)
	{
		case INTRO_SCREEN:
            defines::CopyString(theHUD->introScreenText, "Press Enter To Proceed", 256);
		case RESTART_SCREEN:
			levelTimer = 0; // level has not yet begun
			levelTimeRemaining = totalLevelTime;
			if(Globals::Instance().keys.GetKeyPressed(eKeyCode_RETURN))
			{
				mGameTime.Pause(false); //paused = 0;// not paused
				theHUD->currentHUDState = MAIN_GAME; // HUD in main game state
			}
			else
			{
				break; // game not started, don't run MAIN_GAME code
			}
		case REPLAY:
		case MAIN_GAME:

			if(Globals::Instance().keys.GetKeyPressed(eKeyCode_P))
			{
				mGameTime.Pause(!mGameTime.IsPaused());
			}

#ifdef _DEBUG
			if(Globals::Instance().keys.GetKeyPressed(eKeyCode_2))
			{
				ReplayManager::instance()->SetWholeLevelReplayRepeating(!ReplayManager::instance()->GetWholeLevelReplayRepeating());
			}
#endif

			if (!mGameTime.IsPaused())
			{
				++mFrameCounter;

				replay->IncrementFrame();				

#ifdef DETERMINISTIC_REPLAY_DEBUG
				theSprites.checkMovingSpritesPositions(*replay);
				replay->UnsignedIntResult(levelTimer);
#endif
				
				theSprites.processKeyInput();
			}
			break;
		case END_GAME:
			if(Globals::Instance().keys.GetKeyPressed(eKeyCode_RETURN))
			{
				Globals::Instance().main = menuSystem; // set function pointer back to the menu
				Globals::Instance().drawMenu = 1; // draw the menu
				theHUD->reinitialiseHUD(); /*	reinitialise the HUD in preperation for next 
											level */

				levelInProgress = false; // level has finished
			}
			break;
		default: break;
	}	
}

// move all the components of the level
void CLevel::move()
{
	// move all the sprites
	if (!mGameTime.IsPaused())
	{ 
		theSprites.moveAll(mGameTime.GetTimeDeltaMilli(), mGameTime.GetTimeDeltaSeconds());		
	}
}

void CLevel::updateReplay()
{
	if (!mGameTime.IsPaused())
	{
		theSprites.saveReplayUpdate(mFrameCounter);
	}
}

// look for collisions
void CLevel::collisionDetection()
{	
	if (!mGameTime.IsPaused()){ theSprites.collisionDetection();}
	theHUD->update(mGameTime.GetTimeDeltaSeconds()); // update values in the HUD after moving

	/*	if Nebulus' y value is less than 0 he has fallen in the water, less than -10 means nebulus clearly falls into the water */
	if( !mGameTime.IsPaused() && 
		(theNebulus->position.y < -50.0f || 
		levelTimeRemaining <= 0 || 
		theNebulus->levelCompleted ||
		(ReplayManager::instance()->IsReplayingEndLevel() && ReplayManager::instance()->HasEndReplayFinished()) ))
	{
		if (!ReplayManager::instance()->IsReplaying())
		{
			if (theNebulus->levelCompleted)
			{
#ifdef _DEBUG
				replay->SetWholeLevelReplayRepeating(false);
#endif
				startWholeLevelReplay = true; // replay whole level
				theNebulus->levelCompleted = false; // the replay will set this back to true again on finish
				return;
			}
			else // died
			{
				Globals::Instance().sound.PlaySound( SOUNDS_LOSELIFE, false ); // play lose life sound once
				theNebulus->lives -= 1;
				if (theNebulus->position.y < -50.0f) // died through falling off tower
				{
					theHUD->restartScreenSubTextXScreenPerCent = 0.1f;
                    defines::CopyString(theHUD->restartScreenSubText, "You Fell Off The Tower, Press Enter To Play", 256);

					reStartLevel(ReplayManager::eRSEnd);// replay end level
					return;
				}

				theHUD->restartScreenSubTextXScreenPerCent = 0.1f;
				defines::CopyString(theHUD->restartScreenSubText, "You Ran Out Of Time, Press Enter To Play", 256);
				// died by running out of time - no replay
			}			
		}
		
		if (theNebulus->levelCompleted)
		{
#ifdef _DEBUG
			// this needs to be done here before we try to get stored input (which will crash as it's outside the stored array bounds)
			if (ReplayManager::instance()->IsReplayingWholeLevel() && ReplayManager::instance()->IsFinalFrameOfWholeLevelReplay() &&			
				ReplayManager::instance()->GetWholeLevelReplayRepeating())
			{
				startWholeLevelReplay = true; // replay whole level
				theNebulus->levelCompleted = false; // the replay will set this back to true again on finish
				return;
			}
#endif

			theHUD->currentHUDState = END_GAME;
            defines::CopyString(theHUD->endScreenText, "Level Completed", 256); // set text
			theHUD->endScreenTextXScreenPerCent = 0.07f;
			
            SPRINTF(theHUD->endScreenSubText,
				"Level score %d, time bonus %d, Total score %d", 
				/*	time bonus is a per centage of 10,000 the longer the level has taken 
					to complete the lower the time bonus */	
				theNebulus->score, 10000-int(((float)levelTimer/(float)totalLevelTime)*10000.0f), 
				theNebulus->score+(10000-int(((float)levelTimer/(float)totalLevelTime)*10000.0f)));

			/*	make the text centred, the text will be between 42 and 54 characters, 
				see what percentage of 12 (54-42) it is */
			float perCent = (strlen(theHUD->endScreenSubText)-42.0f)/(54.0f-42.0f);

			// 0.12f for 42 character, 0.001f for 54 characters
			theHUD->endScreenSubTextXScreenPerCent = 0.12f - (perCent * (0.12f-0.001f));

			mGameTime.Pause(true); //paused = 1; // stop the game
		}
		else
		{
			if (theNebulus->lives < 0) // no lives left, go back to menu screen
			{
				theHUD->currentHUDState = END_GAME;
                defines::CopyString(theHUD->endScreenText, "Game Over", 256); // set text
				theHUD->endScreenTextXScreenPerCent = 0.2f; 

				// print a different message depending on the type of death
				if (theNebulus->position.y < -50.0f) 
				{
					theHUD->endScreenSubTextXScreenPerCent = 0.28f;
					defines::CopyString(theHUD->endScreenSubText, "You Fell Off The Tower", 256);
				}

				if (levelTimeRemaining <= 0) 
				{
					theHUD->endScreenSubTextXScreenPerCent = 0.3f;
					defines::CopyString(theHUD->endScreenSubText, "You Ran Out Of Time", 256);
				}
				mGameTime.Pause(true); //paused = 1; // stop the game
			}
			else // set everything back to beginning and start again
			{
				reStartLevel(ReplayManager::eRSMax); // start the level again 
			}
		}
	}

	if (!mGameTime.IsPaused() && !replay->IsReplayingEndLevel())
	{
		CVector nebulusXZPosition = theNebulus->position;
		nebulusXZPosition.y = 0.0f;

		theScene.theCamera->mPriorityTypes.Remove(CameraPriorityTypes::CamTypeAndData(CamType::eCITunnel, 1));
		//	toggle between camera FAR_OUT and TUNNEL_CAM modes as Nebulus goes in and out of a tunnel
		if (nebulusXZPosition.getMagnitude() < (theScene.tower->towerWallPos - (theNebulus->elipsoidRadiusVector.x*1.5f)) && 
			0 != theNebulus->GetTunnel())
		{
			if (theScene.theCamera->cameraMode != TUNNEL_CAM){
				theScene.theCamera->setCameraMode(TUNNEL_CAM);}

			theScene.theCamera->mPriorityTypes.Add(CameraPriorityTypes::CamTypeAndData(CamType::eCITunnel, 1));			
		}
		else
		{
			if (theScene.theCamera->cameraMode != FAR_OUT){
				theScene.theCamera->setCameraMode(FAR_OUT);}
		}
	}
}

void DrawShadowMapRender();

// draw all the components of the level
void CLevel::draw()
{
#ifdef USE_SHADERS
	ShadowMapping::PreDepthTextureRender();
	Draw3DScene();
	ShadowMapping::PostDepthTextureRender();

	ShadowMapping::PreSceneRender();
#endif	

    if (!mGameTime.IsPaused())
    {
        theScene.positionCamera(mGameTime.GetTimeDeltaSeconds());
    } // position scene
    else
    {
        theScene.positionCamera(0.0f);
    }
        
#if (_DEBUG && USE_SHADERS)
    if (drawShadowMap)
    {
        DrawShadowMapRender();
    }
    else
        
#endif
    {
        Draw3DScene();
    }

#ifdef USE_SHADERS
	ShadowMapping::PostSceneRender();
#endif	

	// finally draw the HUD last
	theHUD->draw();
}

void CLevel::Draw3DScene()
{
	if (!mGameTime.IsPaused())
	{
		theSprites.drawAll(mGameTime.GetTimeDeltaMilli(), false);	
	} // draw the non transparent sprites first
	else
	{
		theSprites.drawAll(0, false);
	}

	levelOctree.drawEndNodes(); // draw all the geometry which is in the end nodes in the frustum

	if (!mGameTime.IsPaused())
	{
		theSprites.drawAll(mGameTime.GetTimeDeltaMilli(), true);	
	} // draw the transparent sprites after drawing the tower
	else
	{
		theSprites.drawAll(0, true);
	}
}

#if (_DEBUG && USE_SHADERS)
void DrawShadowMapRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear screen and depth buffer
	Globals::Instance().viewMatrixStack.LoadIdentity(); // reset the current modelview matrix
	Globals::Instance().modelMatrixStack.LoadIdentity(); // reset the current modelview matrix

	// set the screen to orthographic view
	CMenu::setToOrthograthicView();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)

	MeshBuffer backgroundMesh;

		// verts
	CVector	centre(0.5f*Globals::Instance().windowWidth, 0.5f*Globals::Instance().windowHeight, 0.0f);
	CVector dimensions(Globals::Instance().windowWidth, Globals::Instance().windowHeight, 0.0f);
	GLfloat vertexPositionArray[] = {
		ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f,
		ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
		ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
		ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f
	};
	backgroundMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));

	float vertexTexCoordsArray[] = 
	{
		0.0f, 0.0f, 
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	backgroundMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));

	unsigned short vertexIndecisArray[] = 
	{
		0, 1, 2,
		0, 2, 3
	};
	backgroundMesh.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
	//backgroundMesh.SetColor(CVector4(1.0f, 0.0f, 0.0f, 1.0f));
	backgroundMesh.SetTexture(ShadowMapping::GetDepthTexture());

	backgroundMesh.Draw();

	// set the screen back to perspective view
	CMenu::setToPerspectiveView();
}
#endif

void CLevel::CheckStartWholeLevelReplay()
{
	if (startWholeLevelReplay)
	{
		reStartLevel(ReplayManager::eRSWholeLevel); // replay whole level
	}

#ifdef _DEBUG
	if(Globals::Instance().keys.GetKeyPressed(eKeyCode_1))
	{
		replay->SetWholeLevelReplayRepeating(true);
		reStartLevel(ReplayManager::eRSWholeLevel); // replay whole level
	}

	// this needs to be done here before we try to get stored input (which will crash as it's outside the stored array bounds)
	if (ReplayManager::instance()->IsReplayingWholeLevel() && ReplayManager::instance()->IsFinalFrameOfWholeLevelReplay())
	{
		if (ReplayManager::instance()->GetWholeLevelReplayRepeating())
		{
			reStartLevel(ReplayManager::eRSWholeLevel); // replay whole level
		}
		else if (INTRO_SCREEN != theHUD->currentHUDState && 
				END_GAME != theHUD->currentHUDState)
		{
			reStartLevel(ReplayManager::eRSMax);
		}
	}
#endif
}




		



