//CExitScreen.h - header for the exit screen class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CExitScreen_h_
#define _CExitScreen_h_

//header files---------
#include "CTowerSprite.h"
//---------------------

//defines--------------
//---------------------

class CExitScreen : public CTowerSprite
{
	private:
//-----private variables-----------
		void initialise( void);
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		CExitScreen(); // constructor
		// Constructor with model parameter and tower position
		CExitScreen(CModel * aModel, int towerPosition); 
		~CExitScreen(); // destructor
		// reinitialise sprite (empty as sprite does not move)
		void reinitialise( void){}

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CExitScreen

#endif // ifndef _CExitScreen_h_ 