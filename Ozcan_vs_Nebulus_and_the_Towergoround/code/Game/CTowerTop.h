//CTowerTop.h - header for the tower top class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CTowerTop_h_
#define _CTowerTop_h_

//header files---------
#include "CTowerSprite.h"
//---------------------

//defines--------------
//---------------------

class CTowerTop : public CTowerSprite
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CTowerTop(CModel * aModel); 
		~CTowerTop(); // destructor
		// reinitialise to starting state (empty as sprites characteristics never change)
		void reinitialise( void){}

		// overloaded operators

//-----------------------------------------------------------
}; // end class CTowerTop

#endif // ifndef _CTowerTop_h_ 