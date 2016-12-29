//CMenu.h - header for the menu class (includes CMenuOption)

#ifndef _CMenu_h_
#define _CMenu_h_

//header files---------
#include "Math/CVector.h"
#include "Math/CMatrix.h"
#include "FileReading/texture.h"
#include "Rendering/MeshBuffer.h"
#include "Rendering/Text.h"

//system includes------------
#include <vector> // include this to use the vector class
//---------------------------

//---------------------

//external definitions-------
//---------------------------

//defines--------------
//---------------------

// external variables ----
#ifndef USE_SHADERS
extern HDC			hDC;
#endif
// -----------------------

class CMenuOption
{
	public:
// private variables -------------------
#ifdef USE_SHADERS
		Text* text;
#else
		GLuint	base; // base display list for the font
#endif
// -------------------------------------

// public variables --------------------
		char screenText[100]; // the text which appears on the screen for this option
		CVector screenXYPos;  // the x, y on screen position of the selection
// -------------------------------------

// public methods ----------------------
		CMenuOption()
#ifdef USE_SHADERS
			: text(NULL)
#endif
		{
#ifndef USE_SHADERS
			base = 0; // this will be changed when a display list is generated
#endif
		}

		virtual int action( void) = 0; // pure virtual action function

		void buildMenuOptionFont(int height)
		{
#ifdef USE_SHADERS
			text = new Text(screenText, screenXYPos.x, screenXYPos.y, 
							Text::HorizontalAlignment::center, Text::VirticalAlignment::middle, CVector4(0.0f, 0.0f, 0.5f, 1.0f), (float)height);
#else
			HFONT	font,	 // windows Font ID
					oldfont; // used For Good House Keeping

			base = glGenLists(256); // storage For 256 Characters

			font = CreateFont(height, 0, 0, 0, FW_BOLD, FALSE,	FALSE,						
							FALSE, ANSI_CHARSET, OUT_TT_PRECIS,					
							CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,			
							FF_DONTCARE|DEFAULT_PITCH, "NEWFOUNDLAND");		

			oldfont = (HFONT)SelectObject(hDC, font); // selects The Font We Want
			/*	glGetError is called here to clear the openGL error buffer as
				wglUseFontBitmaps will return false and fail if their is anything 
				in the openGL error buffer */
			glGetError();
			wglUseFontBitmaps(hDC, 0, 255, base); // builds 256 Characters Starting At Character 0
			SelectObject(hDC, oldfont);	// selects the previous font to be used
			DeleteObject(font);	// delete The Font
#endif
		}

		virtual void draw( void) // draws the menu option text
		{
#ifdef USE_SHADERS
			if (NULL != text)
			{
				text->Draw();
			}
#else
			// set colour
			glColor3f(0.0f, 0.0f, 0.5f);
			// position The Text On The Screen (0 - windowWidth, 0 - windowHeight)
			glRasterPos2f(screenXYPos.x, screenXYPos.y);
			
			glPushAttrib(GL_LIST_BIT); // pushes The Display List Bits
			glListBase(base); // sets The Base Character to 32
			// draws The Display List Text
			glCallLists(strlen(screenText), GL_UNSIGNED_BYTE, screenText); 
			glPopAttrib(); // pops The Display List Bits
#endif
		}

		virtual ~CMenuOption() // destructor
		{
#ifdef USE_SHADERS
			if (NULL != text)
			{
				delete text;
			}
#else
			glDeleteLists(base, 256); // delete All 256 Characters
#endif
		}
// -------------------------------------
}; // end class CMenuOption

class CMenu
{
	private:
//-----private variables-----------
#ifdef USE_SHADERS
		MeshBuffer curserMesh;
		MeshBuffer backgroundMesh;
#endif
//---------------------------------

	private:
		bool LoadBackground(char * backgroundFileName);
		bool LoadCurser(char * curserFileName);

	public:
//----public variables-------------
		int background, // the background image for this menu (bound to an int)
			curser,		// the curser image bound to an int

			subMenuSelected, // the current sub menu selected
			menuOptionSelected; // the current menu option selected

		CVector centre,		// menus centre position
				dimensions; // menus dimensions

		std::vector<CMenuOption*> menuOptions; // this menu's options 
		std::vector<CMenu*> subMenus; // the sub menus which comprise this menu
//---------------------------------

//----public methods---------------
		CMenu(); // constructor
		virtual ~CMenu(); // destructor

		void keyInput( void); // respond to input from the key board
		void draw( void); // draw the menu background and its options
		// load background and curser textures
		int Load(char * bckgrndFileName, char * curserFileName);
		// draw the background image
		void drawBackGroundImage( void);
		// draw the curser
		void drawCurser( void);
		// set sub menu positions and dimensions
		void setSubMenuPositionsAndDimensions();
		// set options positions and dimensions
		void setOptionsPositionsAndDimensions();
		// set this menus dimensions and centre point
		void setMenuDimensionsAndCentrePoint(CVector *centre, CVector *dimensions);
		// delete the menu
		void deleteMenu( void);

		// virtual methods
		/* perform an action when the menu is left (pure virtual so must be 
													implemented in derived class) */
		virtual void leaveMenu( void) = 0;


		// static methods 
		static void setToOrthograthicView();
		static void setToPerspectiveView();
#ifdef USE_SHADERS
		static void SetPerspectiveProjectionMatrix();
		static void SetOrthographicProjectionMatrix();

		static CMatrix GetOrthographicProjectionMatrix();
#endif
		static CMatrix GetPerspectiveProjectionMatrix();

		// overloaded operators

//---------------------------------
}; // end class CMenu

#endif // ifndef _CMenu_h_ 