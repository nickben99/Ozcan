//CPowerUpGrow.h - header for the power up double size class

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CPowerUpGrow_h_
#define _CPowerUpGrow_h_

//header files---------
#include "CPowerUp.h"
#include "Game/defines.h"
//---------------------

//defines--------------	
#define GROW_TIME_PERIOD 0.3f /* nebulus will grow from normal to double size and back 
								 again over the course of 300 milliseconds */
//---------------------

//external definitions --
//-----------------------

class CPowerUpGrow : public CPowerUp
{
	private:
//-----private variables-----------
		// used to record Nebulus's original top ground speed frames per second
		int originaltopGroundSpeedFramePerSecond;
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CPowerUpGrow(CModel * aModel, char towerSegmentIdentifier); 
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
		virtual ~CPowerUpGrow(); // destructor

		// overloaded operators

//-----------------------------------------------------------
}; // end class CPowerUpGrow

#endif // ifndef _CPowerUpGrow_h_ 