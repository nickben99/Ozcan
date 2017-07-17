// main.cpp - code for main loop

//--- System includes -----------------
//-------------------------------------

//--- Header files --------------------
#include "main.h" // the header file
#include "stdio.h"
#include "Math/CVector.h"
#include "Math/CMatrix.h"
#include "assert.h"
#include "Math/CPlane.h"
#include "Game/defines.h"
#include "CMainMenu.h"
#include "FileReading/texture.h"
#include "Game/Globals.h"
#if _DEBUG
	#include "Debug/DebugMenu.h"
#endif
//--------------------------------------

//global variables----------------------
//--------------------------------------

float shadowCoordBias = 0.005f;
float visibilityDecrement = 0.23f;

#if _DEBUG
bool showSpecularHighlights = true;
bool usePoisson = true;
bool uUseAmbient = true;
bool uUseDiffuse = true;
bool uUseEmmissive = true;
bool uUseMaterialShininessOverride = false;
float materialShininessOverride = 0.05f;

bool uUseAmbientOverride = false;
CVector uAmbientOverride(1.0f, 1.0f, 1.0f);
bool uUseDiffuseOverride = false;
CVector uDiffuseOverride(1.0f, 1.0f, 1.0f);
bool uUseEmmissiveOverride = false;
CVector4 uEmmissiveOverride(1.0f, 1.0f, 1.0f, 1.0f);
bool uUseSpecularOverride = false;
CVector uSpecularOverride(1.0f, 1.0f, 1.0f);

void InitShaderDebugMenu()
{
	DebugMenu::DebugMenuItemContainer* shaders = new DebugMenu::DebugMenuItemContainer("shaders");
		
	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("show specular highlights", &showSpecularHighlights));
	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("use poisson", &usePoisson));
	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseAmbient", &uUseAmbient));
	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseDiffuse", &uUseDiffuse));
	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseEmmissive", &uUseEmmissive));
	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseMaterialShininessOverride", &uUseMaterialShininessOverride));
	shaders->AddVariable(new DebugMenu::FloatDebugMenuItem("materialShininessOverride", &(materialShininessOverride), 0.001f, -0.001f));

	CVector4 increment(0.01f, 0.01f, 0.01f, 0.01f);
	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseAmbientOverride", &uUseAmbientOverride));
	shaders->AddVariable(new DebugMenu::CVectorDebugMenuItem("uAmbientOverride", &(uAmbientOverride), increment.v3, -increment.v3));

	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseDiffuseOverride", &uUseDiffuseOverride));
	shaders->AddVariable(new DebugMenu::CVectorDebugMenuItem("uDiffuseOverride", &(uDiffuseOverride), increment.v3, -increment.v3));

	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseEmmissiveOverride", &uUseEmmissiveOverride));
	shaders->AddVariable(new DebugMenu::CVector4DebugMenuItem("uEmmissiveOverride", &(uEmmissiveOverride), increment, -increment));

	shaders->AddVariable(new DebugMenu::BoolDebugMenuItem("uUseSpecularOverride", &uUseSpecularOverride));
	shaders->AddVariable(new DebugMenu::CVectorDebugMenuItem("uSpecularOverride", &uSpecularOverride, increment.v3, -increment.v3));

	shaders->AddVariable(new DebugMenu::FloatDebugMenuItem("shadowCoordBias", &shadowCoordBias, 0.001f, -0.001f));
	shaders->AddVariable(new DebugMenu::FloatDebugMenuItem("visibilty decrement", &visibilityDecrement, 0.001f, -0.001f));

	Globals::Instance().debugMenu.AddVariable(shaders);
}
#endif

void SetShaderParams()
{
	Globals::Instance().gl.SetUniformFloat(Globals::Instance().gl.GetUniformLocation("shadowCoordBias"), shadowCoordBias);
	Globals::Instance().gl.SetUniformFloat(Globals::Instance().gl.GetUniformLocation("uVisibilityDecrement"), visibilityDecrement);

#ifdef _DEBUG
	//Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uShowSpecularHighlights"), showSpecularHighlights);
	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uPoisson"), usePoisson);
	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseAmbient"), uUseAmbient);
	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseDiffuse"), uUseDiffuse);
	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseEmmissive"), uUseEmmissive);
	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseMaterialShininessOverride"), uUseMaterialShininessOverride);
	Globals::Instance().gl.SetUniformFloat(Globals::Instance().gl.GetUniformLocation("materialShininessOverride"), materialShininessOverride);

	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseAmbientOverride"), uUseAmbientOverride);
	Globals::Instance().gl.SetUniformVector3(Globals::Instance().gl.GetUniformLocation("uAmbientOverride"), uAmbientOverride);

	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseDiffuseOverride"), uUseDiffuseOverride);
	Globals::Instance().gl.SetUniformVector3(Globals::Instance().gl.GetUniformLocation("uDiffuseOverride"), uDiffuseOverride);

	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseEmmissiveOverride"), uUseEmmissiveOverride);
	Globals::Instance().gl.SetUniformVector4(Globals::Instance().gl.GetUniformLocation("uEmmissiveOverride"), uEmmissiveOverride);

	Globals::Instance().gl.SetUniformBool(Globals::Instance().gl.GetUniformLocation("uUseSpecularOverride"), uUseSpecularOverride);
	Globals::Instance().gl.SetUniformVector3(Globals::Instance().gl.GetUniformLocation("uSpecularOverride"), uSpecularOverride);
#endif
}

void InitShaderParams()
{
#if _DEBUG
	InitShaderDebugMenu();
#endif
	SetShaderParams();
}

// initialise the game menus
bool initGameMenus()
{
	InitShaderParams();

	// set the main menus dimensions and centre point
	CVector centre(Globals::Instance().windowWidth*0.5f, Globals::Instance().windowHeight*0.5f, 0.0f),
			dimensions(Globals::Instance().windowWidth, Globals::Instance().windowHeight, 0.0f);
	Globals::Instance().mpMainMenu->setMenuDimensionsAndCentrePoint(&centre, &dimensions);

	// load the background and curser for the main menu
	if (!Globals::Instance().mpMainMenu->Load()){
		return( false );} // menus didn't load succesfully

	// set the positions of the main menus options
	Globals::Instance().mpMainMenu->setOptionsPositionsAndDimensions();

	return(true); // menus initialised succesfully
}

// delete all game objects
void deleteGameObjects() 
{
	/*	delete level (would be automatically deleted, but done here before the 
		texture vector is deleted below) */
	Globals::Instance().currentLevel.deleteCurrentLevel();

	/*	the HUD is a singleton and its destructor will be called automatically 
		by the programme.  It is being explicitly called here so its textures are deleted 
		before the texture vector is deleted below */
	Globals::Instance().currentLevel.theHUD->deleteHUD();

	/*	delete the main menu (would be automatically deleted, but done here before the 
		texture vector is deleted below) */ 
	Globals::Instance().mpMainMenu->deleteMenu();

	// clear the vector of texture indices
	clearTextureObjectVector();
}

// draw the openGL graphics
GLboolean drawGLScene(GLint level)
{
	/*	if the previous level has ended or this is the first 
		run of the game */
	if (!Globals::Instance().currentLevel.islevelInProgress())
	{
		Globals::Instance().currentLevel.deleteCurrentLevel(); // delete everything from the previous level
		if(!Globals::Instance().currentLevel.loadLevel(level)){ // load a new level
			return(false);} // error loading level
		// get camera singleton instances
		Globals::Instance().currentLevel.theHUD->draw(); // just draw the HUD this frame
	}
	else // level is running
	{		
		Globals::Instance().currentLevel.processKeyInput(); // perform actions on sprites based on key input
		Globals::Instance().currentLevel.updateTime(); // update level clock
		Globals::Instance().currentLevel.move(); // move everything in the level
		Globals::Instance().currentLevel.collisionDetection(); // look for collisions between objects

		if (ReplayManager::instance()->IsReplaying())
		{
			Globals::Instance().currentLevel.updateReplay();
		}
		Globals::Instance().currentLevel.draw(); // draw everything in the level
	}// end if level in progress

	if (Globals::Instance().currentLevel.islevelInProgress() && ReplayManager::instance()->IsRecording())
	{		
		Globals::Instance().currentLevel.updateReplay();
	}
	Globals::Instance().currentLevel.CheckStartWholeLevelReplay();

#ifdef _DEBUG
	SetShaderParams();
#endif

	return(true); // everything worked succesfully
}

// main function
int gameMain(int level)									
{
	return drawGLScene(level); // draw the scene
}

// the menu system
int menuSystem(int) // parameter is not used
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear screen and depth buffer
	Globals::Instance().viewMatrixStack.LoadIdentity(); // reset the current view matrix
	Globals::Instance().modelMatrixStack.LoadIdentity(); // reset the current model matrix

	// get input to the menu
	Globals::Instance().mpMainMenu->keyInput();

	// set the screen to orthographic view
	CMenu::setToOrthograthicView();

#ifdef USE_SHADERS
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)
#endif

	// draw the menus
	if (Globals::Instance().drawMenu){Globals::Instance().mpMainMenu->draw();}
	
	// set the screen back to perspective view
	CMenu::setToPerspectiveView();

	return( 1 ); // everything went ok
}
