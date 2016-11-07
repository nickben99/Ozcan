//CCollapsingStep.h - header for the collapsing step class

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CCollapsingStep_h_
#define _CCollapsingStep_h_

//header files---------
#include "CTowerStep.h"
//---------------------

//defines--------------
#define COLLAPSE_WAIT_PERIOD 0.3f /* time between nebulus hitting step and step actually
									collapsing */	
//---------------------

//external definitions --
//-----------------------

class CCollapsingStep : public CTowerStep
{
	private:
//-----private variables-----------
		float timeTillCollapse; // timer untill the step collapses
		bool hasBeenSteppedOn;
//---------------------------------

//-----private methods-------------
		void initialise( void);
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CCollapsingStep(CModel * aModel, int towerPosition); 
		~CCollapsingStep(); // destructor
		// move the collapsing step
		void move(int timeChange, GLfloat perCentOfSecond); // move the step
		// colliion response
		void collisionResponse(	CSprite * collidedWith,
								float perCentOfMoveComplete, 
								CVector * polyintersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc);

		// reinitialise the sprite to its starting position
		void reinitialise( void);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CCollapsingStep

#endif // ifndef _CCollapsingStep_h_ 