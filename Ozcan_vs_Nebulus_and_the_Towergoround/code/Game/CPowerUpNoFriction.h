//CPowerUpNoFriction.h - header for the no friction power up class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CPowerUpNoFriction_h_
#define _CPowerUpNoFriction_h_

//header files---------
#include "CPowerUp.h"
#include "Game/defines.h"
//---------------------

//defines--------------	
//---------------------

//external definitions --
//-----------------------

class CPowerUpNoFriction : public CPowerUp
{
	private:
//-----private variables-----------
		// keep a record of the original friction value for Nebulus
		float	nebulusOriginalFrictionValue;	
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CPowerUpNoFriction(CModel * aModel, char towerSegmentIdentifier); 
		void reinitialise(char towerSegmentIdentifier); // initialisation
		// perform action by the power-up
		void move(int timeChange, GLfloat perCentOfSecond);
		// draw this sprite
		void draw(int timeChange);
		// collision response
		void collisionResponse(	CSprite * collidedWith,
								float perCentOfMoveComplete, 
								CVector * polyintersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * velocity);

		// virtual methods
		virtual ~CPowerUpNoFriction(); // destructor

		// overloaded operators

//-----------------------------------------------------------
}; // end class CPowerUpNoFriction

#endif // ifndef _CPowerUpNoFriction_h_ 