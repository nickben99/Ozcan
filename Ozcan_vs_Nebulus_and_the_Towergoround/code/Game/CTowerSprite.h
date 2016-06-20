//CTowerSprite.h - header for the tower sprite class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CTowerSprite_h_
#define _CTowerSprite_h_

//header files---------
#include "CSprite.h"
#include "CTower.h"
#include "Math/CMatrix.h"
#include "Game/defines.h"
//---------------------

//defines--------------
//---------------------

class CTowerSprite : public CSprite
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables--------------
		CTower * theTower; // referance to the singleton tower instance
		int startingTowerPosition; // the starting position on the tower
//----------------------------------

//----public methods----------------------------------------
		CTowerSprite(); // constructor
		CTowerSprite(CModel * aModel); // overloaded constructor
		// Constructor with model parameter and tower position
		CTowerSprite(CModel * aModel, int towerPosition); 
		virtual void draw(int timeChange); // draw this sprite

		// virtual methods
		virtual ~CTowerSprite(); // destructor
		virtual void initialise(int towerPosition); // initialisation
		// reinitialise sprite to starting state
		virtual void reinitialise( void);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CTowerSprite

#endif // ifndef _CTowerSprite_h_ 