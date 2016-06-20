//CPowerUpSpeed.h - header for the power up double speed class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CPowerUpSpeed_h_
#define _CPowerUpSpeed_h_

//header files---------
#include "CPowerUp.h"
#include "Game/defines.h"
//---------------------

//defines--------------	
#define SPEED_REDUCTION_TIME_PERIOD 1.0f /* reduce speed back to normal over the 
											course of a second */
//---------------------

//external definitions --
//-----------------------

class CPowerUpSpeed : public CPowerUp
{
	private:
//-----private variables-----------
		// keep a record of the original values for Nebulus which will be changed
		float	nebulusOriginalThrustValue,
				nebulusOriginalMaxMidAirSpeedValue;	
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CPowerUpSpeed(CModel * aModel, char towerSegmentIdentifier); 
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
		virtual ~CPowerUpSpeed(); // destructor

		// overloaded operators

//-----------------------------------------------------------
}; // end class CPowerUpSpeed

#endif // ifndef _CPowerUpSpeed_h_ 