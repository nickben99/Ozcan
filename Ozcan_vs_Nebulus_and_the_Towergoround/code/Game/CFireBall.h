//CFireBall.h - header for the fireball class

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CFireBall_h_
#define _CFireBall_h_

//header files---------
#include "CSprite.h"
#include "CTower.h"
#include "Math/CMatrix.h"
#include "Game/defines.h"
#include "CNebulus.h"
//---------------------

//defines--------------
#define FIRE_BALL_TIME_PERIOD 1.5f // fire ball burns out after 1.5 seconds
#define DISAPPEAR_TIME_PERIOD 0.1f // the time the fire ball takes to disappear
//---------------------

// external definitions
//---------------------

class CFireBall : public CSprite
{
	private:
//-----private variables-----------
		bool currentlyInSurfaceContact; /*	records whether the fire ball is currently 
											in contact with a surface or not */
		float	upThrust,
				fireBallTimer; // for destroying the fire abll after a set length of time
//---------------------------------

//-----private methods-------------
		void initialise();
//---------------------------------

	public:
//----public variables--------------
		CNebulus* theNebulus; // pointer to nebulus
//----------------------------------

//----public methods----------------------------------------
		CFireBall(); // constructor
		CFireBall(CModel * aModel); // overloaded constructor
		~CFireBall(); // destructor
		void keyInput(); // input control to the fire ball
		void move(int timeChange, GLfloat perCentOfSecond); // move the fire ball
		// draw the sprite
		void draw(int timeChange);
		void collisionResponse(CSprite * collidedWith, 
								float perCentOfMoveComplete,
								CVector * polyintersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc);	/*	response to hitting a 
														perticular sprite */
		// reinitialise sprite to starting state
		void reinitialise( void);

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CFireBall

#endif // ifndef _CFireBall_h_ 