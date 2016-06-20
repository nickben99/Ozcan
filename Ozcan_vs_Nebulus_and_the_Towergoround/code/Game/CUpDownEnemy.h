//CUpDownEnemy.h - header for the up/down enemy class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CUpDownEnemy_h_
#define _CUpDownEnemy_h_

//header files---------
#include "CTowerStep.h"
#include "Audio/SoundsEnum.h"
//---------------------

//defines--------------	
#define UP_DOWN_ENEMY_ROTATE_SPEED PI*1.5f /*	the up down enemy rotates 270 degrees
												every second */
//---------------------

//external definitions --
//-----------------------

class CUpDownEnemy : public CTowerStep
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
		CUpDownEnemy(CModel * aModel, int towerPosition); 
		~CUpDownEnemy(); // destructor
		// move the sprite
		void move(int timeChange, GLfloat perCentOfSecond);
		// collision response
		void collisionResponse(	CSprite * collidedWith, 
								float perCentOfMoveComplete,  // for evironment collision
								CVector * polyIntersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc); // for environment collision
		// set sprite back to starting state
		void reinitialise( void);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CUpDownEnemy

#endif // ifndef _CUpDownEnemy_h_ 