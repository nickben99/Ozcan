// CBouncingSideToSideEnemy.h - header for the bouncing side to side enemy class

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CBouncingSideToSideEnemy_h_
#define _CBouncingSideToSideEnemy_h_

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

class CBouncingSideToSideEnemy : public CTowerStep
{
	private:
//-----private variables-----------
		CParticleSystem particles; /*	particle system for the sprite, if it exsplodes or 
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
		CBouncingSideToSideEnemy(CModel * aModel, int towerPosition); 
		~CBouncingSideToSideEnemy(); // destructor
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
		// reinitialise the sprite to its starting position
		void reinitialise( void);

		void saveReplayUpdate(ReplayData&, bool);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CBouncingSideToSideEnemy

#endif // ifndef _CBouncingSideToSideEnemy_h_ 