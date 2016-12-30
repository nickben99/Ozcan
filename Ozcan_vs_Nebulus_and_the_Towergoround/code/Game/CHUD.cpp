//CHUD.cpp, a Head Up Display HUD class

// system includes --------
#include <math.h>
#include <time.h>
#ifdef _WINDOWS
#include <glew-1.12.0\include\GL/glew.h>
#endif
//-------------------------

// header files -----------
#include "CHUD.h"
#include "CMenu.h"
#include "Globals.h"
#include "Game/defines.h"
#include "Game/ReplayManager.h"
#include <System/Interfaces/SystemInterface.h>
//-------------------------

//extern HWND hWnd;

extern GLuint TextureLoad(const char *filename, bool alpha, GLenum minfilter, GLenum magfilter, GLenum wrap);

float CalculateRestartScreenTop(float screenHeight)
{
	return ToShaderUIY(0.3f*screenHeight);
}

float CalculateRestartScreenBottom(float screenHeight)
{
	return ToShaderUIY(0.7f*screenHeight);
}

// creates singleton HUD instance
CHUD* CHUD::instance() 
{
	static CHUD HUDInstance; // create static HUD instance once
    return &HUDInstance; // return the address of the HUD instance
}

// constructor
CHUD::CHUD( void)
#ifdef USE_SHADERS
	: font(NULL)
	, largeFont(NULL)
#endif
{ 
	reinitialiseHUD();
	introScreenBackground = 0;
#ifndef USE_SHADERS	
	fontBase = largeFontBase = 0; // initialise
#endif
}

// reinitialise the HUD
void CHUD::reinitialiseHUD( void) // set back to starting state
{
	currentHUDState = INTRO_SCREEN; // hud starts on intro screen
    defines::CopyString(introScreenText, "Loading...", 256);
	powerUpPerCentRemaining = 0.0f;
}

// destructor
CHUD::~CHUD()
{
	deleteHUD();
}

// delete the HUD display lists and graphics
void CHUD::deleteHUD( void)
{
#ifdef USE_SHADERS
	if (NULL != font)
	{
		delete font;
		font = NULL;
	}

	if (NULL != largeFont)
	{
		delete largeFont;
		largeFont = NULL;
	}
#else
	glDeleteLists(fontBase, 256); // delete All 256 Characters
	glDeleteLists(largeFontBase, 256); 
#endif

	deleteTexture(introScreenBackground); // free openGL texture

#ifdef USE_SHADERS
	introScreenBuffer.Delete();
	loadingBarBackgroundBuffer.Delete();
	loadingBarForegroundBuffer.Delete();

	powerUpBarBackgroundBuffer.Delete();
	powerUpBarForegroundBuffer.Delete();

	restartScreenBackgroundBuffer.Delete();
	endGameScreenBackgroundBuffer.Delete();
#endif
}

// load the huds graphics and font
int CHUD::loadHUDGraphicsAndFont(float width, float height)
{
	screenWidth = width;
	screenHeight = height; // record window dimensions

	// set up the normal font
	letterHeight = (int)((30.0f/640.0f)*width); // letter height based on width of screen
	int largeFontLetterHeight = (int)((75.0f/640.0f)*width);

#ifdef USE_SHADERS
	if (NULL == font)
	{
		font = new Text();
		font->SetSize((float)letterHeight);
	}

	if (NULL == largeFont)
	{
		largeFont = new Text();
		largeFont->SetSize((float)largeFontLetterHeight);
	}
#else
	HFONT	font,	 // windows Font ID
			oldfont; // used For Good House Keeping

	fontBase = glGenLists(256); // storage For 256 Characters

	font = CreateFont(letterHeight, 0, 0, 0, FW_BOLD, FALSE,	FALSE,						
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,					
					CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,			
					FF_DONTCARE|DEFAULT_PITCH, "Adelaide");	
	
	oldfont = (HFONT)SelectObject(hDC, font); // selects The Font We Want
	/*	glGetError is called here to clear the openGL error buffer as
		wglUseFontBitmaps will return false and fail if their is anything 
		in the openGL error buffer */
	glGetError();
	wglUseFontBitmaps(hDC, 0, 255, fontBase); // builds 256 Characters Starting At Character 0
	SelectObject(hDC, oldfont);	// selects the previous font to be used
	DeleteObject(font);	// delete The Font

	// set up the large font used on the restart screen

	largeFontBase = glGenLists(256); // storage For 256 Characters

	font = CreateFont(largeFontLetterHeight, 0, 0, 0, FW_BOLD, FALSE,	FALSE,						
					FALSE, ANSI_CHARSET, OUT_TT_PRECIS,					
					CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,			
					FF_DONTCARE|DEFAULT_PITCH, "Adelaide");		

	oldfont = (HFONT)SelectObject(hDC, font); // selects The Font We Want
	/*	glGetError is called here to clear the openGL error buffer as
		wglUseFontBitmaps will return false and fail if their is anything 
		in the openGL error buffer */
	glGetError();
	wglUseFontBitmaps(hDC, 0, 255, largeFontBase); // builds 256 Characters Starting At Character 0
	SelectObject(hDC, oldfont);	// selects the previous font to be used
	DeleteObject(font);	// delete The Font
#endif

	char buffer[256];
    SPRINTF(buffer, "%simages/introScrn1.bmp", GetDirectoryPath()); // create file name with path

	introScreenBackground = // bind the intro screen Background texture
		TextureLoad(buffer, GL_FALSE, GL_LINEAR, GL_LINEAR, GL_REPEAT);

	if (introScreenBackground <= 0){ // if their were problems loading the background
		return(0);} // unsuccesful load

#ifdef USE_SHADERS
	// introScreenBuffer -----------------------------------------------------------------------------------------------------------
	{
		GLfloat vertexPositionArray[] = {
			ToShaderUIX(0.0f), ToShaderUIY(0.0f), 0.0f,
			ToShaderUIX(0.0f), ToShaderUIY(screenHeight), 0.0f,
			ToShaderUIX(screenWidth), ToShaderUIY(screenHeight), 0.0f,
			ToShaderUIX(screenWidth), ToShaderUIY(0.0f), 0.0f
		};

		introScreenBuffer.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
		introScreenBuffer.SetColor(CVector4(1.0f, 1.0f, 1.0f, 1.0f));

		float vertexTexCoordsArray[] =
		{
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
		};
		introScreenBuffer.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
		introScreenBuffer.SetTexture(introScreenBackground);

		unsigned short vertexIndecisArray[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		introScreenBuffer.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
	}

	// loading Bar -----------------------------------------------------------------------------------------------------------
	{
		float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;
		CVector4 backgroundColor, foregroundColor;
		GetLoadingBarDimensionsAndColor(left, right, top, bottom, backgroundColor, foregroundColor);

		GLfloat vertexPositionArray[] = {
			ToShaderUIX(left), ToShaderUIY(top), 0.0f,
			ToShaderUIX(left), ToShaderUIY(bottom), 0.0f,
			ToShaderUIX(right), ToShaderUIY(bottom), 0.0f,
			ToShaderUIX(right), ToShaderUIY(top), 0.0f
		};
		loadingBarBackgroundBuffer.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
		loadingBarForegroundBuffer.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float), true);

		loadingBarBackgroundBuffer.SetColor(backgroundColor);
		loadingBarForegroundBuffer.SetColor(foregroundColor);
        
        float vertexTexCoordsArray[] =
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        loadingBarBackgroundBuffer.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
        loadingBarForegroundBuffer.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));

		unsigned short vertexIndecisArray[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		loadingBarBackgroundBuffer.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
		loadingBarForegroundBuffer.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
	}

	// power-up Bar -----------------------------------------------------------------------------------------------------------
	{
		float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;
		CVector4 backgroundColor, foregroundColor;
		GetPowerUpBarDimensionsAndColor(left, right, top, bottom, backgroundColor, foregroundColor);

		GLfloat vertexPositionArray[] = {
			ToShaderUIX(left), ToShaderUIY(top), 0.0f,
			ToShaderUIX(left), ToShaderUIY(bottom), 0.0f,
			ToShaderUIX(right), ToShaderUIY(bottom), 0.0f,
			ToShaderUIX(right), ToShaderUIY(top), 0.0f
		};
		powerUpBarBackgroundBuffer.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
		powerUpBarForegroundBuffer.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float), true);

		powerUpBarBackgroundBuffer.SetColor(backgroundColor);
		powerUpBarForegroundBuffer.SetColor(foregroundColor);
        
        float vertexTexCoordsArray[] =
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        powerUpBarBackgroundBuffer.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
        powerUpBarForegroundBuffer.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));

		unsigned short vertexIndecisArray[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		powerUpBarBackgroundBuffer.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
		powerUpBarForegroundBuffer.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
	}

	// restartScreenBackgroundBuffer -----------------------------------------------------------------------------------------------------------
	{
		GLfloat vertexPositionArray[] = {
			ToShaderUIX(0.0f), CalculateRestartScreenTop(screenHeight), 0.0f,
			ToShaderUIX(0.0f), CalculateRestartScreenBottom(screenHeight), 0.0f,
			ToShaderUIX(screenWidth), CalculateRestartScreenBottom(screenHeight), 0.0f,
			ToShaderUIX(screenWidth), CalculateRestartScreenTop(screenHeight), 0.0f
		};

		restartScreenBackgroundBuffer.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
		restartScreenBackgroundBuffer.SetColor(CVector4(1.0f, 1.0f, 1.0f, 180.0f/255.0f));
        
        float vertexTexCoordsArray[] =
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        restartScreenBackgroundBuffer.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));

		unsigned short vertexIndecisArray[] = 
		{
			0, 1, 2,
			0, 2, 3
		};
		restartScreenBackgroundBuffer.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
	}

	// endGameScreenBackgroundBuffer -----------------------------------------------------------------------------------------------------------
	{
		GLfloat vertexPositionArray[] = {
			ToShaderUIX(0.0f), ToShaderUIY(0.0f), 0.0f,
			ToShaderUIX(0.0f), ToShaderUIY(screenHeight), 0.0f,
			ToShaderUIX(screenWidth), ToShaderUIY(screenHeight), 0.0f,
			ToShaderUIX(screenWidth), ToShaderUIY(0.0f), 0.0f
		};

		endGameScreenBackgroundBuffer.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
		endGameScreenBackgroundBuffer.SetColor(CVector4(0.0f, 0.0f, 0.0f, 1.0f));

        float vertexTexCoordsArray[] =
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        endGameScreenBackgroundBuffer.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
        
		unsigned short vertexIndecisArray[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		endGameScreenBackgroundBuffer.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
	}
#endif

	return( 1 ); // everything loaded succesfully
}

// update any quantities displayed on the HUD
void CHUD::update(float)
{
	// fader.update(delta);
}

// draw the HUD to the screen
void CHUD::draw( void)
{
	CMenu::setToOrthograthicView(); // go into orthograthic view
#ifndef USE_SHADERS
	Globals::Instance().gl.GLDisable(GL_BLEND); // disable blending
#else
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending

	Globals::Instance().gl.GLDisable(GL_LIGHTING);
#endif

	switch( currentHUDState )
	{
		case INTRO_SCREEN:
			introScreenDraw();
			break;
		case MAIN_GAME: 
			mainGameDraw();
			break;
		case REPLAY:
			ReplayDraw();
			break;
		case RESTART_SCREEN:
			drawReStartScreen();
			break;
		case END_GAME:
			drawEndGameScreen();
			break;
		default: break;
	}

#if (_DEBUG && USE_SHADERS)
	Globals::Instance().debugMenu.Update();
	Globals::Instance().debugMenu.Draw();
#endif

#ifndef USE_SHADERS
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending
#else
	Globals::Instance().gl.GLEnable(GL_LIGHTING);
#endif
	CMenu::setToPerspectiveView(); // go into perspective view
}

// run the intro screen drawing code
void CHUD::introScreenDraw( void)
{
	// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
	float	xPos = 0.5f*screenWidth,
			yPos = (470.0f/480)*screenHeight;

#ifdef USE_SHADERS
	introScreenBuffer.Draw();

	font->SetText(introScreenText, ToShaderUIX(xPos), ToShaderUIY(yPos), Text::HorizontalAlignment::left, Text::VirticalAlignment::bottom, CVector4(255/255, 125/255, 29/255, 255/255));
	font->Draw();
#else
	Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // enable texturing
	glBindTexture(GL_TEXTURE_2D, introScreenBackground); // bind texture required
	glColor3ub(255, 255, 255); // set drawing colour to white
	glBegin(GL_QUADS);		// draw background texture
		glTexCoord2f(0.0, 0.0); glVertex2f(	0.0f, 
											0.0f);
		glTexCoord2f(0.0, 1.0); glVertex2f(	0.0f, 
											screenHeight);
		glTexCoord2f(1.0, 1.0); glVertex2f(	screenWidth,
											screenHeight);
		glTexCoord2f(1.0, 0.0); glVertex2f(	screenWidth, 
											0.0f);
	glEnd();
	Globals::Instance().gl.GLDisable(GL_TEXTURE_2D); // disable texturing

	// set colour
	glColor3ub(255, 125, 29);

	glRasterPos2f(xPos, yPos);

	printString(fontBase, introScreenText);
#endif
}

void CHUD::drawIntroScreenLoadingBar( float floadPerCentComplete_ )
{
	CMenu::setToOrthograthicView(); // go into orthograthic view	

	float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;
	CVector4 background, foreground;
	GetLoadingBarDimensionsAndColor(left, right, top, bottom, background, foreground);

	float fPercentRemainingRightSide = 0.0f;

#ifdef USE_SHADERS
	DrawBarChartWithShaders(loadingBarBackgroundBuffer, loadingBarForegroundBuffer, left, right, top, bottom, floadPerCentComplete_, fPercentRemainingRightSide);
#else
	unsigned char	backColour[ 3 ] = { (char)(background.v3.x*255.0f), (char)(background.v3.y*255.0f), (char)(background.v3.z*255.0f) }, 
					frontColour[ 3 ] = { (char)(foreground.v3.x*255.0f), (char)(foreground.v3.y*255.0f), (char)(foreground.v3.z*255.0f) };

	drawBarChart( left, right, top, bottom, floadPerCentComplete_, 
					backColour, frontColour,
					fPercentRemainingRightSide );
#endif
	CMenu::setToPerspectiveView(); // go into perspective view
}

void CHUD::GetLoadingBarDimensionsAndColor(float& left, float& right, float& top, float& bottom, CVector4& background, CVector4& foreground)
{
	left = (40.0f/640.0f)*screenWidth;
	right = left+(240.0f/640)*screenWidth;
	top = (455.0f/480.0f)*screenHeight;
	bottom = top +(10.0f/480.0f)*screenHeight;

	background = CVector4(0, 0, 0, 1.0f); 
	foreground = CVector4(0, 184.0f/255.0f, 0, 1.0f);
}

void CHUD::GetPowerUpBarDimensionsAndColor(float& left, float& right, float& top, float& bottom, CVector4& background, CVector4& foreground)
{
	left = (10.0f/640)*screenWidth;
	top = ((50.0f+(letterHeight*1.5f))/480)*screenHeight; 

	//POINT p;
	//if (GetCursorPos(&p))
	//{			
	//	if (ScreenToClient(hWnd, &p))
	//	{
	//		//p.x and p.y are now relative to hwnd's client area
	//		left = (float)p.x;
	//		top = (float)p.y;
	//	}
	//}

	right = left+(110.0f/640)*screenWidth; 		
	bottom = top + letterHeight; 

	background = CVector4(238.0f/255.0f, 28.0f/255.0f, 0, 1.0f); 
	foreground = CVector4(73.0f/255.0f, 5.0f/255.0f, 165.0f/255.0f, 1.0f);
}

void CHUD::ReplayDraw()
{
	GLboolean wasTextureEnabled = glIsEnabled(GL_TEXTURE_2D); 
	if (wasTextureEnabled)
	{
		Globals::Instance().gl.GLDisable(GL_TEXTURE_2D); // disable texturing
	}
	
	const int NumReplayTextChars = 128;
	char replayText[NumReplayTextChars];

#ifdef _DEBUG
	bool wholeLevelReplay = ReplayManager::instance()->IsReplayingWholeLevel();
	bool repeating = ReplayManager::instance()->GetWholeLevelReplayRepeating();

	if (wholeLevelReplay)
	{
        SPRINTF(replayText, "REPLAY REPLAY - whole - %s", (repeating) ? "repeat" : "non repeat");
	}
	else
	{
        defines::CopyString(replayText, "REPLAY REPLAY - end", NumReplayTextChars);
	}
#else
	defines::CopyString(replayText, "REPLAY REPLAY", NumReplayTextChars); // print the string
#endif

	float theTime = ((float)Globals::Instance().timer.time())*0.001f; // convert to seconds
	const float kPeriod = 8.0f;
	theTime *= kPeriod;
	float alpha = ((float)sin(theTime)+1.0f)/2.0f; // get normalized output from sin 

#ifdef USE_SHADERS
	float	xPos = 0.5f*screenWidth,
			yPos = (450.0f/480)*screenHeight;

	font->SetText(replayText, ToShaderUIX(xPos), ToShaderUIY(yPos), 
				Text::HorizontalAlignment::center, Text::VirticalAlignment::bottom, CVector4(238.0f/255.0f, 28.0f/255.0f, 0.0f, alpha));
	font->Draw();
#else
	float	xPos = (230.0f/640)*screenWidth,
			yPos = (450.0f/480)*screenHeight;

	// set colour
	glColor4ub(238, 28, 0, (GLubyte)(alpha*255.0f));
	// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
	glRasterPos2f(xPos, yPos); // set position

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)

	printString(fontBase, replayText); 

	Globals::Instance().gl.GLDisable(GL_BLEND);
#endif

	if (wasTextureEnabled)
	{
		Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // disable blending
	}
}

// run the main game drawing code
void CHUD::mainGameDraw()
{
	// print time remaining
#ifdef USE_SHADERS
	float	xPos = 0.5f*screenWidth,
			yPos = (50.0f/480)*screenHeight;

	font->SetText(ToShaderUIX(xPos), ToShaderUIY(yPos), 
				Text::HorizontalAlignment::center, Text::VirticalAlignment::bottom, CVector4(73.0f/255.0f, 5.0f/255.0f, 165.0f/255.0f, 1.0f));
	font->SetText("Time Remaining 999"); // put in a dummy time (but it will be the same each frame, so the centralized text won't move around)

	CVector topLeft;
	CVector bottomRight;
	font->GetTextBox(topLeft, bottomRight);

	// set the text again before drawing with the correct time, so the text will be central, but won't change 
	// position slightly in response to time text change
	font->SetText(topLeft.x, bottomRight.y, 
				Text::HorizontalAlignment::left, Text::VirticalAlignment::bottom, CVector4(73.0f/255.0f, 5.0f/255.0f, 165.0f/255.0f, 1.0f));
	font->SetText("Time Remaining %3.0f", (*levelTimer*0.004f));
	font->Draw();

	// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
	xPos = (10.0f/640)*screenWidth,
	yPos = ((50.0f+letterHeight)/480)*screenHeight;

	font->SetText(ToShaderUIX(xPos), ToShaderUIY(yPos), 
				Text::HorizontalAlignment::left, Text::VirticalAlignment::bottom, CVector4(73.0f/255.0f, 5.0f/255.0f, 165.0f/255.0f, 1.0f));
	font->SetText("Score %d", theNebulus->score);
	font->Draw();

	if (MAIN_GAME == currentHUDState && GameTime::Instance().IsPaused())
	{
		// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
		xPos = 0.5f*screenWidth;
		yPos = (450.0f/480)*screenHeight;

		font->SetText(ToShaderUIX(xPos), ToShaderUIY(yPos), 
					Text::HorizontalAlignment::center, Text::VirticalAlignment::bottom, CVector4(238.0f/255.0f, 28.0f/255.0f, 0.0f, 1.0f));
		font->SetText("Press 'P' To Resume");
		font->Draw();
	}

	// print lives remaining
	xPos = (630.0f/640)*screenWidth;
	yPos = ((50.0f+letterHeight)/480)*screenHeight;

	font->SetText(ToShaderUIX(xPos), ToShaderUIY(yPos), 
				Text::HorizontalAlignment::right, Text::VirticalAlignment::bottom, CVector4(73.0f/255.0f, 5.0f/255.0f, 165.0f/255.0f, 1.0f));
	font->SetText("Lives x%d", theNebulus->lives);
	font->Draw();
#else
	// set colour
	glColor3ub(238, 28, 0);
	// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
	float	xPos = (230.0f/640)*screenWidth,
			yPos = (50.0f/480)*screenHeight;

	glRasterPos2f(xPos, yPos); // set position

	printString(fontBase, "Time Remaining %3.0f", (*levelTimer*0.004f)); // print the string

	// print score
	// set colour
	glColor3ub(73, 5, 165);
	// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
	xPos = (10.0f/640)*screenWidth,
	yPos = ((50.0f+letterHeight)/480)*screenHeight;
	
	glRasterPos2f(xPos, yPos);

	printString(fontBase, "Score %d", theNebulus->score);

	if (MAIN_GAME == currentHUDState && GameTime::Instance().IsPaused())
	{
		glColor3ub(238, 28, 0);
		// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
		xPos = (230.0f/640)*screenWidth;
		yPos = (450.0f/480)*screenHeight;
		glRasterPos2f(xPos, yPos); // set position

		printString(fontBase, "Press 'P' To Resume"); 
	}

	// print lives remaining
	glColor3ub(238, 28, 0);
	xPos = (540.0f/640)*screenWidth;
	yPos = ((50.0f+letterHeight)/480)*screenHeight;
	glRasterPos2f(xPos, yPos);
	printString(fontBase, "Lives x%d", theNebulus->lives);
#endif

	//powerUpPerCentRemaining = 1.0f;
	if (powerUpPerCentRemaining > 0.0f)
	{
		float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;
		CVector4 background, foreground;
		GetPowerUpBarDimensionsAndColor(left, right, top, bottom, background, foreground);

		float fPercentRemainingRightSide = 0.0f;

#ifdef USE_SHADERS
		DrawBarChartWithShaders(powerUpBarBackgroundBuffer, powerUpBarForegroundBuffer, left, right, top, bottom, powerUpPerCentRemaining, fPercentRemainingRightSide);
		font->SetText(ToShaderUIX(fPercentRemainingRightSide), ToShaderUIY(top+((bottom-top)*0.5f)), 
								Text::HorizontalAlignment::left, Text::VirticalAlignment::middle, CVector4(foreground, 1.0f));
		font->SetText("Power-Up");
		font->Draw();
#else
		unsigned char backColour[ 3 ] = { (char)(background.v3.x*255.0f), (char)(background.v3.y*255.0f), (char)(background.v3.z*255.0f) }, 
						frontColour[ 3 ] = { (char)(foreground.v3.x*255.0f), (char)(foreground.v3.y*255.0f), (char)(foreground.v3.z*255.0f) };

		drawBarChart( left, right, top, bottom, powerUpPerCentRemaining, 
						backColour, frontColour, fPercentRemainingRightSide );

		// draw text
		glRasterPos2f( fPercentRemainingRightSide, bottom-((bottom-top)*0.3f) );
		printString(fontBase, "Power-Up");
#endif
	}
}

// print a string
#ifndef USE_SHADERS
void CHUD::printString(int charactersRequired, char * string, ...)
{
	char outputString[256];
	va_list vlist; // va_list
	
	va_start(vlist, string); // set to beginning of argument list
	vsprintf_s(outputString, string, vlist); // print formatted string to thisString
	va_end(vlist); // reset argument pointer

	glPushAttrib(GL_LIST_BIT); // pushes The Display List Bits
	glListBase(charactersRequired); // sets The base Character
	// draws The Display List Text
	glCallLists(strlen(outputString), GL_UNSIGNED_BYTE, outputString); 
	glPopAttrib(); // pops The Display List Bits
}
#endif

#ifdef USE_SHADERS
// draw a bar chart
void CHUD::DrawBarChartWithShaders(MeshBuffer& background, MeshBuffer& foreground, float leftSide, float rightSide, 
						float top, float bottom, float perCent, float &quadTwoRightSide)
{
	background.Draw();

	// work out right side of quad based on percentage
	quadTwoRightSide = leftSide+(perCent*(rightSide - leftSide));

	float vertexPositionArray[] = {
		ToShaderUIX(leftSide), ToShaderUIY(top), 0.0f,
		ToShaderUIX(leftSide), ToShaderUIY(bottom), 0.0f,
		ToShaderUIX(quadTwoRightSide), ToShaderUIY(bottom), 0.0f,
		ToShaderUIX(quadTwoRightSide), ToShaderUIY(top), 0.0f
	};
	foreground.UpdateVertexArray(vertexPositionArray);

	foreground.Draw();
}
#else
// draw a bar chart
void CHUD::drawBarChart(float leftSide, float rightSide, 
						float top, float bottom, float perCent, 
						unsigned char *backColour, unsigned char *frontColour,
						float &quadTwoRightSide )
{
	glColor3ubv( backColour ); // set drawing colour
	glBegin(GL_QUADS);		// draw background texture
		glVertex2f(	leftSide, top);
		glVertex2f(	leftSide, bottom);
		glVertex2f(	rightSide, bottom);
		glVertex2f(	rightSide, top);
	glEnd();

	// work out right side of quad based on percentage
	quadTwoRightSide = leftSide+(perCent*(rightSide - leftSide));

	glColor3ubv( frontColour ); // set drawing colour
	glBegin(GL_QUADS);		// draw background texture
		glVertex2f(	leftSide, top);
		glVertex2f(	leftSide, bottom);
		glVertex2f(	quadTwoRightSide, bottom);
		glVertex2f(	quadTwoRightSide, top);
	glEnd();
}
#endif

// print out the message when the game restarts after Nebulus dies
void CHUD::drawReStartScreen()
{
#ifndef USE_SHADERS
	Globals::Instance().gl.GLDisable(GL_TEXTURE_2D); // disable texturing
#endif
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
	Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)

#ifdef USE_SHADERS
	restartScreenBackgroundBuffer.Draw();
#else
	glColor4ub(255, 255, 255, 180); // set drawing colour to black
	glBegin(GL_QUADS);		// draw background
		glVertex2f(	0.0f, 0.3f*screenHeight);
		glVertex2f(	0.0f, 0.7f*screenHeight);
		glVertex2f(	screenWidth, 0.7f*screenHeight);
		glVertex2f(	screenWidth, 0.3f*screenHeight);
	glEnd();
	Globals::Instance().gl.GLDisable(GL_BLEND);
#endif

#ifdef USE_SHADERS
	float restartScreenTop = CalculateRestartScreenTop(screenHeight);
	float restartScreenBottom = CalculateRestartScreenBottom(screenHeight);
	float restartScreenCenterY = restartScreenTop + ((restartScreenBottom-restartScreenTop)*0.5f);

	// initislly position of text doesn't matter, as we just want to get the text box dimensions
	largeFont->SetText(0.0f, 0.0f, Text::HorizontalAlignment::center, Text::VirticalAlignment::top, CVector4(73.0f/255.0f, 5.0f/255.0f, 165.0f/255.0f, 1.0f));
	largeFont->SetText("%d %s", theNebulus->lives, ((1 == theNebulus->lives) ? "Life Remaining" : "Lives Remaining"));

	CVector mainTextBoxTopLeft;
	CVector mainTextBoxBottomRight;
	largeFont->GetTextBox(mainTextBoxTopLeft, mainTextBoxBottomRight);
	float mainTextBoxHeight = mainTextBoxBottomRight.y - mainTextBoxTopLeft.y;

	// initislly position of text doesn't matter, as we just want to get the text box dimensions
	font->SetText(0.0f, 0.0f, Text::HorizontalAlignment::center, Text::VirticalAlignment::top, CVector4(0.0f, 0.0f, 0.0f, 1.0f));
	font->SetText(restartScreenSubText);

	CVector subTextBoxTopLeft;
	CVector subTextBoxBottomRight;
	font->GetTextBox(subTextBoxTopLeft, subTextBoxBottomRight);
	float subTextBoxHeight = subTextBoxBottomRight.y - subTextBoxTopLeft.y;

	// calculate entire size of text in order to place it in the center of the restart screen background
	float entireTextHeight = mainTextBoxHeight + subTextBoxHeight;
	float yDrawPos = restartScreenCenterY - (0.5f*entireTextHeight);
	float xDrawPos = ToShaderUIX(0.5f*screenWidth);

	largeFont->SetText(xDrawPos, yDrawPos);
	largeFont->Draw();

	yDrawPos += mainTextBoxHeight;
	font->SetText(xDrawPos, yDrawPos);	
	font->Draw();
#else
	glColor3ub(73, 5, 165); // set drawing colour
	float	xPos = 0.05f*screenWidth,
			yPos = 0.5f*screenHeight;
	if (1 == theNebulus->lives)
	{
		xPos = 0.08f*screenWidth;
		glRasterPos2f(xPos, yPos);
		printString(largeFontBase, "%d Life Remaining", theNebulus->lives);
	}
	else
	{
		glRasterPos2f(xPos, yPos);
		printString(largeFontBase, "%d Lives Remaining", theNebulus->lives);
	}
	
	glColor3ub(0, 0, 0); // set drawing colour
	yPos+=letterHeight;
	xPos=(restartScreenSubTextXScreenPerCent*screenWidth);
	glRasterPos2f(xPos, yPos);
	printString(fontBase, restartScreenSubText);
#endif
}

// draw the end game screen
void CHUD::drawEndGameScreen( void)
{
#ifdef USE_SHADERS
	endGameScreenBackgroundBuffer.Draw();

	float screenCenterY = ToShaderUIY(0.5f*screenHeight);

	// initislly position of text doesn't matter, as we just want to get the text box dimensions
	largeFont->SetText(0.0f, 0.0f, Text::HorizontalAlignment::center, Text::VirticalAlignment::top, CVector4(176.0f/255.0f, 195.0f/255.0f, 244.0f/255.0f, 1.0f));
	largeFont->SetText(endScreenText);

	CVector mainTextBoxTopLeft;
	CVector mainTextBoxBottomRight;
	largeFont->GetTextBox(mainTextBoxTopLeft, mainTextBoxBottomRight);
	float mainTextBoxHeight = mainTextBoxBottomRight.y - mainTextBoxTopLeft.y;

	// initially position of text doesn't matter, as we just want to get the text box dimensions
	font->SetText(0.0f, 0.0f, Text::HorizontalAlignment::center, Text::VirticalAlignment::top, CVector4(176.0f/255.0f, 195.0f/255.0f, 244.0f/255.0f, 1.0f));
	font->SetText(endScreenSubText);

	CVector subTextBoxTopLeft;
	CVector subTextBoxBottomRight;
	font->GetTextBox(subTextBoxTopLeft, subTextBoxBottomRight);
	float subTextBoxHeight = subTextBoxBottomRight.y - subTextBoxTopLeft.y;

	// calculate entire size of text in order to place it in the center of the restart screen background
	float entireTextHeight = mainTextBoxHeight + (2.0f*subTextBoxHeight); // multipling by 2 as we the font has two lines of text
	float yDrawPos = screenCenterY - (0.5f*entireTextHeight);
	float xDrawPos = ToShaderUIX(0.5f*screenWidth);

	largeFont->SetText(xDrawPos, yDrawPos);
	largeFont->Draw();

	yDrawPos += mainTextBoxHeight;
	font->SetText(xDrawPos, yDrawPos);	
	font->Draw();

	yDrawPos += subTextBoxHeight;
	font->SetText(xDrawPos, yDrawPos, Text::HorizontalAlignment::center, Text::VirticalAlignment::top, CVector4(255.0f/255.0f, 125.0f/255.0f, 29.0f/255.0f, 1.0f));
	font->SetText("Press Enter To Return To The Menu Screen");
	font->Draw();
#else
	Globals::Instance().gl.GLDisable(GL_TEXTURE_2D); // disable texturing
	glColor3ub(0, 0, 0); // set drawing colour to black
	glBegin(GL_QUADS);		// draw background
		glVertex2f(	0.0f, 0.0f);
		glVertex2f(	0.0f, screenHeight);
		glVertex2f(	screenWidth, screenHeight);
		glVertex2f(	screenWidth, 0.0f);
	glEnd();

	glColor3ub(176, 195, 244); // set drawing colour
	float	xPos = endScreenTextXScreenPerCent*screenWidth,
			yPos = 0.5f*screenHeight;
	glRasterPos2f(xPos, yPos);
	printString(largeFontBase, endScreenText);

	glColor3ub(176, 195, 244); // set drawing colour
	xPos = (endScreenSubTextXScreenPerCent*screenWidth);
	yPos+=letterHeight;
	glRasterPos2f(xPos, yPos);
	printString(fontBase, endScreenSubText);

	glColor3ub(255, 125, 29); // set drawing colour
	xPos = (0.12f*screenWidth);
	yPos+=letterHeight*2.0f;
	glRasterPos2f(xPos, yPos);
	printString(fontBase, "Press Enter To Return To The Menu Screen");
#endif
}