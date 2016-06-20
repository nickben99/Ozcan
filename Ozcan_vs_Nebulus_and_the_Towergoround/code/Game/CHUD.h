//CHUD.h - header for the Head Up Display (HUD) class

// system includes ----------
#include <windows.h>		// header file for windows
#include <stdio.h>			// header file for standard input/output
#include <gl\gl.h>			// header file for the OpenGL32 library
#include <gl\glu.h>			// header file for the GLu32 library
// --------------------------

#ifndef _CHUD_h_
#define _CHUD_h_

//header files-------------------
#include "FileReading/texture.h" // for loading textures
#include "CNebulus.h"
#include "CHUDFader.h"
#ifdef USE_SHADERS
#include "Rendering/MeshBuffer.h"
#include "Rendering\Text.h"
#endif
//-------------------------------

class CVector4;

// defines ----------------------
enum hudState // the different states the HUD can be in
{
	INTRO_SCREEN, 
	RESTART_SCREEN,
	MAIN_GAME,
	REPLAY,
	END_GAME
};
// ------------------------------

class CHUD
{
	private:
//-----private variables-----------
#ifdef USE_SHADERS
		Text* font;
		Text* largeFont;
#else
		int fontBase, // the hud fonts display list
			largeFontBase;
#endif
		int introScreenBackground;
		int letterHeight; // height of HUD font

#ifdef USE_SHADERS	
		MeshBuffer introScreenBuffer, 
					loadingBarBackgroundBuffer, loadingBarForegroundBuffer, 
					powerUpBarBackgroundBuffer, powerUpBarForegroundBuffer, 
					restartScreenBackgroundBuffer, 
					endGameScreenBackgroundBuffer;
#endif

		float	screenWidth,
				screenHeight; // record window dimensions
//---------------------------------

//-----private methods--------------------
			CHUD( void); // Constructor
			// run the intro screen drawing code
			void introScreenDraw( void);
			// run the main game drawing code
			void mainGameDraw();

			void ReplayDraw();
#ifndef USE_SHADERS
			// draw a bar chart
			void drawBarChart( float leftSide, float rightSide, float top, float bottom, 
								float perCent, unsigned char *backColour, 
								unsigned char *frontColour, float &quadTwoRightSide );
#else
			// draw a bar chart
			void DrawBarChartWithShaders(MeshBuffer& background, MeshBuffer& foreground, float leftSide, float rightSide, 
										float top, float bottom, float percent, float &quadTwoRightSide);
#endif
			void GetLoadingBarDimensionsAndColor(float& left, float& right, float& top, float& bottom, CVector4& background, CVector4& foreground);
			void GetPowerUpBarDimensionsAndColor(float& left, float& right, float& top, float& bottom, CVector4& background, CVector4& foreground);
			// print a string
#ifndef USE_SHADERS
			void printString(int charactersRequired, char * string, ...);
#endif
//----------------------------------------

	public:
//----public variables-------------
		char	introScreenText[256], // text for the introscreen
				restartScreenText[256], // text for the restart screen
				restartScreenSubText[256],
				endScreenText[256], // text for the end game screen
				endScreenSubText[256]; // sub text for the restart screen
		hudState currentHUDState; // the hud state (from hudStates enum above)
		CNebulus *theNebulus; // pointer to singleton nebulus
		int *levelTimer; // referance to the CLevel levelTimer
		float	powerUpPerCentRemaining, // perCent of timed power up remaining
				endScreenTextXScreenPerCent, // the x positioning of the end screen text
				endScreenSubTextXScreenPerCent, // the x positioning of the end screen sub text
				restartScreenTextXScreenPerCent, 
				restartScreenSubTextXScreenPerCent;
		CHUDFader fader;
//---------------------------------

//----public methods----------------------------------------
		static CHUD* instance(); // get the singleton CHUD instance
		~CHUD(); // destructor

		int loadHUDGraphicsAndFont(float width, float height); // load the huds graphics and font
		void update(float delta); // update any quantities displayed on the HUD
		void draw( void); // draw the HUD to the screen
		// draw the loading bar for the intro screen
		void drawIntroScreenLoadingBar( float floadPerCentComplete_ );
		// print out the message when the game restarts after Nebulus dies
		void drawReStartScreen();
		// reinitialise the HUD
		void reinitialiseHUD( void); // set back to starting state
		// delete the HUD display lists and graphics
		void deleteHUD( void);
		// draw the end game screen
		void drawEndGameScreen( void);

		inline float GetScreenWidth()
		{ 
			return screenWidth;
		}
		
		inline float GetScreenHeight()
		{
			return screenHeight;
		}
//-----------------------------------------------------------
}; // end class CHUD

#endif // ifndef _CHUD_h_ 