//CTowerBottom.h - header for the tower bottom class

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CTowerBottom_h_
#define _CTowerBottom_h_

//header files---------
#include "CTowerSprite.h"
//---------------------

//defines--------------
//---------------------

class CTowerBottom : public CTowerSprite
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
		// Constructor with model parameter
		CTowerBottom(CModel * aModel); 
		~CTowerBottom(); // destructor
		// reinitialise to starting state (empty as sprites characteristics never change)
		void reinitialise( void){}

		// overloaded operators

//-----------------------------------------------------------
}; // end class CTowerBottom

#endif // ifndef _CTowerBottom_h_ 