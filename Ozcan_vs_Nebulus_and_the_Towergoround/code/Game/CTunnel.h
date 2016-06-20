//CTunnel.h - header for the tower tunnel class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CTunnel_h_
#define _CTunnel_h_

//header files---------
#include "CTowerSprite.h"
//---------------------

//defines--------------
//---------------------

class CTunnel : public CTowerSprite
{
	private:
//-----private variables-----------
//---------------------------------

//-----private methods-------------
		void initialise( void);
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter and tower position
		CTunnel(CModel * aModel, int towerPosition); 
		~CTunnel(); // destructor
		// reinitialise to starting state (empty as sprites characteristics never change)
		void reinitialise( void){}

		// overloaded operators

//-----------------------------------------------------------
}; // end class CTunnel

#endif // ifndef _CTunnel_h_ 