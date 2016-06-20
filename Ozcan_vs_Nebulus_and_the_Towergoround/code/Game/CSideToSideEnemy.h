//CSideToSideEnemy.h - header for the side to side enemy class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CSideToSideEnemy_h_
#define _CSideToSideEnemy_h_

//header files---------
#include "CTowerStep.h"
#include "Audio/SoundsEnum.h"
//---------------------

// external variables ----------
// -----------------------------

//defines--------------	
#define COEFFICIENT_OF_RESTITUTION 1.0f // for collisions
//---------------------

//external definitions --
//-----------------------

class CSideToSideEnemy : public CTowerStep
{
	private:
//-----private variables-----------
		CParticleSystem particles; /*	particle system for the sprite, if it explodes or 
										needs a trail in the case of the fire ball */
//---------------------------------

//-----private methods-------------
		void initialise( void);
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter and tower position
		CSideToSideEnemy(CModel * aModel, int towerPosition); 
		~CSideToSideEnemy(); // destructor
		// move the sprite
		void move(int timeChange, GLfloat perCentOfSecond);
		// draw the sprite
		void draw(int timeChange);
		// collision response
		void collisionResponse(	CSprite * collidedWith, 
								float perCentOfMoveComplete,  // for evironment collision
								CVector * polyIntersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc); // for environment collision
		// reinitialise to starting state
		void reinitialise( void);

		void saveReplayUpdate(ReplayData&, bool);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CSideToSideEnemy

#endif // ifndef _CSideToSideEnemy_h_ 