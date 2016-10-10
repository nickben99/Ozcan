//CMenu.cpp, the menu class

// system includes --------
#include <Rendering/OpenGLInclude.h>
//-------------------------

// header files -----------
#include "CMenu.h"
#include "Game/Globals.h"
#include "Game/defines.h"
using namespace std; // using standard namespace

//-------------------------

extern GLuint TextureLoad(const char *filename, bool alpha, GLenum minfilter, GLenum magfilter, GLenum wrap);

// constructor
CMenu::CMenu() // constructor
{
	subMenuSelected = // set sub menu selected
	menuOptionSelected = 0; // set the menu option selected
	background = curser = InvalidTextureID;
}

// destructor
CMenu::~CMenu()
{
	deleteMenu();
}

// delete the menu, its sub menus and contents
void CMenu::deleteMenu( void)
{
	int totalsubMenus = (int)subMenus.size(); // total options

	for (int i = 0; i < totalsubMenus; i++) // go through menu options
	{
		if (subMenus[i] != 0) // delete, if not null
		{
			delete(subMenus[i]); // delete 
			subMenus[i] = 0; // set to null
		}
	}

	int totalOptions = (int)menuOptions.size(); // total options

	for (int i = 0; i < totalOptions; i++) // go through menu options
	{
		if (menuOptions[i] != 0) // delete, if not null
		{
			delete(menuOptions[i]); // delete
			menuOptions[i] = 0; // set to null
		}
	}	

	subMenus.clear(); // clear all the vector lists
	menuOptions.clear();

	// delete the menus background and curser textures
	deleteTexture(background);
	deleteTexture(curser);
	background = curser = InvalidTextureID;

#ifdef USE_SHADERS
	backgroundMesh.Delete();
	curserMesh.Delete();
#endif
}

// set sub menu positions and dimensions
void setSubMenuPositionsAndDimensions()
{
}

// set this menus dimensions and centre point
void CMenu::setMenuDimensionsAndCentrePoint(CVector *theCentre, CVector *theDimensions)
{
	centre = *theCentre; // set centre and dimensions
	dimensions = *theDimensions; 
}

// set options positions and dimensions
void CMenu::setOptionsPositionsAndDimensions()
{
	int totalOptions = (int)menuOptions.size(); // total options

#ifdef USE_SHADERS
	float x	= centre.x;
	float y	= dimensions.y*0.5f; // start half way down the screen
#else
	float	x	= centre.x - (dimensions.x*0.1f), /*	start slightly less than 
														half way horizontally */
			y	= dimensions.y*0.55f; // start slightly more than half way down the screen
#endif

	// work out the height of the menu fonts (based on the width of the menu)
	int menuFontHeight = (int)( (35.0f/640.0f)*dimensions.x );

	for (int i = 0; i < totalOptions; i++)
	{
#ifdef USE_SHADERS
		menuOptions[i]->screenXYPos = CVector(ToShaderUIX(x), ToShaderUIY(y), 0.0f);
#else
		menuOptions[i]->screenXYPos = CVector(x, y, 0.0f);
#endif
		// height of the font is based on the width of the menu
		menuOptions[i]->buildMenuOptionFont(menuFontHeight); // build the menu options font		
		y+=menuFontHeight*1.05f; // the next option will be further down
	}	
}

// respond to input from the key board
void CMenu::keyInput( void)
{
	int totalSubMenus = (int)subMenus.size(); // get amount of sub menus

	// if this menu is made up of sub menus
	if (totalSubMenus > 0)
	{
		// tab focus between sub menus
		if(Globals::Instance().keys.GetKeyPressed(eKeyCode_TAB))
		{
			subMenuSelected++; // go to the next sub menu
	
			// if gone too high, go back to first sub menu
			if (subMenuSelected > totalSubMenus){
				subMenuSelected = 0;}
		}
	
		// call key input on the sub menu in focus
		subMenus[subMenuSelected]->keyInput(); 
	}
	else // this menu does not have sub menus, it has options of its own
	{
		int totalOptions = (int)menuOptions.size(); // get amount of options

		// highlight previous option in the list
		if(Globals::Instance().keys.GetKeyPressed(eKeyCode_UP))
		{
			menuOptionSelected--;
	
			// if gone too low, go to the end of the list
			if (menuOptionSelected < 0){
				menuOptionSelected = (totalOptions-1);}

			Globals::Instance().sound.PlaySound( SOUNDS_MENUCHANGE, false ); // play the menu change sound once
		}
		
		// highlight next option in the list
		if(Globals::Instance().keys.GetKeyPressed(eKeyCode_DOWN))
		{
			menuOptionSelected++;
	
			// if gone too low, go back to first sub menu
			if (menuOptionSelected > (totalOptions-1)){
				menuOptionSelected = 0;}

			Globals::Instance().sound.PlaySound( SOUNDS_MENUCHANGE, false ); // play the menu change sound once
		}

		// perform the action associated with the menu item
		if(Globals::Instance().keys.GetKeyPressed(eKeyCode_RETURN))
		{
			/*	if the menu option returns false it indicates the action will
				result in the menu being exited, so the leaveMenu method will 
				be called */
			if (false == menuOptions[menuOptionSelected]->action()){
				leaveMenu();}
		}
	}
}

// draw the menu background and its options
void CMenu::draw( void)
{
	int totalSubMenus = (int)subMenus.size(); // get amount of sub menus

	if (totalSubMenus > 0) // if this menu is made up of sub menus
	{
		// go through the sub menus
		for (int subMenu = 0; subMenu < totalSubMenus; subMenu++)
		{
			subMenus[subMenu]->draw(); // draw the sub menu
		}
	}
	else // this menu has no sub menus
	{
		// draw the background image
		drawBackGroundImage();

		int totalOptions = (int)menuOptions.size(); // get amount of options

		for (int option = 0; option < totalOptions; option++)
		{
			menuOptions[option]->draw(); // draw the sub menu option
		}

		// draw the curser
		drawCurser();
	}// end if totalSubMenus > 0	
}

// draw the background image
void CMenu::drawBackGroundImage( void)
{
#ifdef USE_SHADERS
	backgroundMesh.Draw();
#else
	Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // enable texturing
	glBindTexture(GL_TEXTURE_2D, background); // bind texture required
	glColor3ub(255, 255, 255); // set drawing colour to white
	glBegin(GL_QUADS);		// draw background texture
		glTexCoord2f(0.0, 0.0); glVertex2f(centre.x - dimensions.x*0.5f, centre.y - dimensions.y*0.5f);
		glTexCoord2f(0.0, 1.0); glVertex2f(centre.x - dimensions.x*0.5f, centre.y + dimensions.y*0.5f);
		glTexCoord2f(1.0f, 1.0); glVertex2f(centre.x + dimensions.x*0.5f, centre.y + dimensions.y*0.5f);
		glTexCoord2f(1.0f, 0.0); glVertex2f(centre.x + dimensions.x*0.5f, centre.y - dimensions.y*0.5f);
	glEnd();
	Globals::Instance().gl.GLDisable(GL_TEXTURE_2D); // disable texturing
#endif
}

// draw the curser
void CMenu::drawCurser( void)
{
#ifdef USE_SHADERS
	CVector optionSelectedXY;
	CVector textBoxBottomRight;
	menuOptions[menuOptionSelected]->text->GetTextBox(optionSelectedXY, textBoxBottomRight);
	optionSelectedXY.y += ((textBoxBottomRight.y-optionSelectedXY.y)*0.5f);
#else
	const CVector &optionSelectedXY = menuOptions[menuOptionSelected]->screenXYPos;
#endif

	// work out the height of the menu fonts (based on the width of the menu)
	float	menuFontHeight = (35.0f/640.0f)*dimensions.x;
	CVector squareCentre(optionSelectedXY.x - (menuFontHeight+(menuFontHeight*0.5f)), optionSelectedXY.y, 0.0f);

#ifdef USE_SHADERS
	CMatrix translation;
	translation.SetMatrixTranslation(CVector4(squareCentre, 1.0f));
	Globals::Instance().modelMatrixStack.PushMatrix();
	Globals::Instance().modelMatrixStack.MultMatrix(translation);
		curserMesh.Draw();
	Globals::Instance().modelMatrixStack.PopMatrix();
#else
	Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // enable texturing
	glBindTexture(GL_TEXTURE_2D, curser); // bind texture required
	glColor3ub(255, 255, 255); // set drawing colour to white
	glBegin(GL_QUADS);		// draw background texture
		glTexCoord2f(0.0, 0.0); glVertex2f(	squareCentre.x - (menuFontHeight*0.5f),
											squareCentre.y);
		glTexCoord2f(0.0, 1.0); glVertex2f(	squareCentre.x - (menuFontHeight*0.5f),
											squareCentre.y + (menuFontHeight*0.5f));
		glTexCoord2f(1.0, 1.0); glVertex2f(	squareCentre.x + (menuFontHeight*0.5f),
											squareCentre.y + (menuFontHeight*0.5f));
		glTexCoord2f(1.0, 0.0); glVertex2f(	squareCentre.x + (menuFontHeight*0.5f),
											squareCentre.y);
	glEnd();
	Globals::Instance().gl.GLDisable(GL_TEXTURE_2D); // disable texturing
#endif
}

// load background image
int CMenu::Load(char * bckgrndFileName, char * curserFileName)
{
	if (!LoadBackground(bckgrndFileName))
	{
		return 0;
	}

	if (!LoadCurser(curserFileName))
	{
		return 0;
	}
	return 1;
}

bool CMenu::LoadBackground(char * bckgrndFileName)
{
	background = // bind the background texture
		TextureLoad(bckgrndFileName, GL_FALSE, GL_LINEAR, GL_LINEAR, GL_REPEAT);

	if (background <= InvalidTextureID){ // if their were problems loading the background
		return(false);} // unsuccesful load

#ifdef USE_SHADERS
	// verts
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
	backgroundMesh.SetTexture(background);
#endif

	return(true); // succesful load
}

bool CMenu::LoadCurser(char * curserFileName)
{
	curser = // bind the curser texture
		TextureLoad(curserFileName, GL_FALSE, GL_LINEAR, GL_LINEAR, GL_REPEAT);

	if (curser <= InvalidTextureID){ // if their were problems loading the curser
		return(false);} // unsuccesful load

#ifdef USE_SHADERS
	float menuFontHeight = (35.0f/640.0f)*dimensions.x;

	GLfloat vertexPositionArray[] = {
		(-menuFontHeight*0.5f), (-menuFontHeight*0.25f), 0.0f, // left top
		(-menuFontHeight*0.5f), (menuFontHeight*0.25f), 0.0f,// left bottom
		(menuFontHeight*0.5f), (menuFontHeight*0.25f), 0.0f, // right bottom

		(-menuFontHeight*0.5f), (-menuFontHeight*0.25f), 0.0f, // left top
		(menuFontHeight*0.5f), (menuFontHeight*0.25f), 0.0f, // right bottom
		(menuFontHeight*0.5f), (-menuFontHeight*0.25f), 0.0f // right top
	};

	curserMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));

	float vertexTexCoordsArray[] = 
	{
		0.0, 0.0f,
		0.0, 1.0f,
		1.0, 1.0f,

		0.0, 0.0f,
		1.0, 1.0f,
		1.0, 0.0f
	};

	curserMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
	curserMesh.SetTexture(curser);
#endif

	return true;
}

// set to orthograthic view
void CMenu::setToOrthograthicView()
{
	Globals::Instance().gl.GLDisable(GL_DEPTH_TEST); // depth testing and lighting
	Globals::Instance().gl.GLDisable(GL_LIGHTING);
#ifdef USE_SHADERS
	Globals::Instance().gl.DontUseDefaultTexture();

	SetOrthographicProjectionMatrix();

	Globals::Instance().viewMatrixStack.PushMatrix();
	Globals::Instance().viewMatrixStack.LoadIdentity();

	Globals::Instance().modelMatrixStack.PushMatrix();
	Globals::Instance().modelMatrixStack.LoadIdentity();
#else
	
	Globals::Instance().gl.GLDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);
			glPushMatrix(); // save projection matrix state
				glLoadIdentity(); // load identity
				// set dimensions of orphographic view
				gluOrtho2D(0, Globals::Instance().windowWidth, Globals::Instance().windowHeight, 0);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix(); // save model-view matrix state
					glLoadIdentity();
#endif
}

// set to perspective view
void CMenu::setToPerspectiveView()
{
#ifdef USE_SHADERS
	CMenu::SetPerspectiveProjectionMatrix();
	Globals::Instance().modelMatrixStack.PopMatrix();
	Globals::Instance().viewMatrixStack.PopMatrix();
#else
				glPopMatrix(); // go to previous model-view matrix
				glMatrixMode(GL_PROJECTION); // manipulating projection matrix
			glPopMatrix(); // go back to previous projection matrix
		glMatrixMode(GL_MODELVIEW); // manipulating model-view matrix
	Globals::Instance().gl.GLEnable(GL_TEXTURE_2D);
#endif
	Globals::Instance().gl.GLEnable(GL_LIGHTING);
	Globals::Instance().gl.GLEnable(GL_DEPTH_TEST); // re-enable depth testing and lighting
}

#ifdef USE_SHADERS
void CMenu::SetPerspectiveProjectionMatrix()
{
	Globals::Instance().gl.SetProjectionMatrix(CMenu::GetPerspectiveProjectionMatrix());
}

void CMenu::SetOrthographicProjectionMatrix()
{
	Globals::Instance().gl.SetProjectionMatrix(GetOrthographicProjectionMatrix());
}

CMatrix CMenu::GetOrthographicProjectionMatrix()
{
	float halfWindowWidth = Globals::Instance().windowWidth/2;
	float halfWindowHeight = Globals::Instance().windowHeight/2;
	return CMatrix::CreateOrthographicProjection(-halfWindowWidth, halfWindowWidth, halfWindowHeight, -halfWindowHeight, -100.0f, 100.0f);
}
#endif

CMatrix CMenu::GetPerspectiveProjectionMatrix()
{
	return CMatrix::CreatePerspectiveProjection(45.0f, Globals::Instance().windowWidth/Globals::Instance().windowHeight, 0.1f, 500.0f);
}
