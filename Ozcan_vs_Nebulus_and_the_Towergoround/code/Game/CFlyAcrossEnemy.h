//CFlyAcrossEnemy.h - header for the tower bottom class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CFlyAcrossEnemy_h_
#define _CFlyAcrossEnemy_h_

//header files---------
#include "CTowerStep.h"
#include "CNebulus.h"
#include "Audio/SoundsEnum.h"
//---------------------

// external variables ----------
// -----------------------------

//defines--------------
#define PERIOD_BETWEEN_GENERATIONS 4.0f /* 4 seconds between each generation of the fly
											across enemy */
#define SHRINK_GROW_PERIOD 0.5f // half a second shrink and grow period	
// parameters sent to the shrink/grow method (whether a shrink or grow is required)
enum 
{
	SHRINK,
	GROW
};
//---------------------

class CFlyAcrossEnemy : public CTowerStep
{
	private:
//-----private variables-----------
		float	regenerationTimer, // timer untill the step collapses
				dormentPeriod, // length of time enemy waits to be regenerated
				anglesTravelledThrough, /*	used to set the fly accross enemy to innactive
											after it has completed a circle of the tower */
				shrinkGrowTimer, // times the shrink and grow of the sprite
				normalScale, // records the normal size of the sprite
				resizingRequired; // whether the sprite needs to grow or shrink currently
		CNebulus* theNebulus; // singleton nebulus pointer

		CParticleSystem particles; /*	particle system for the sprite, if it explodes or 
										needs a trail in the case of the fire ball */
//---------------------------------

//-----private methods-------------
		void initialise( void);
		// shrink or grow the sprite (whichever is required)
		void shrinkOrGrow(int resizingRequired);
//---------------------------------

	public:
//----public variables--------------
		float	towerCircumferance, // the towers circumferance 
				angularVelocity; // speed fly accross enemy travels at
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CFlyAcrossEnemy(CModel * aModel); 
		~CFlyAcrossEnemy(); // destructor
		// move the sprite
		void move(int timeChange, GLfloat perCentOfSecond);
		// draw the sprite
		void draw(int timeChange);
		// collision response
		void collisionResponse(	CSprite * collidedWith, 
								float perCentOfMoveComplete,  
								CVector * polyIntersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc); // for environment collision
		// set the sprite back to its starting state
		void reinitialise( void);

		void saveReplayUpdate(ReplayData& data, bool firstFrame);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CFlyAcrossEnemy

#endif // ifndef _CFlyAcrossEnemy_h_ 