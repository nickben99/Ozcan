//CSlippyStep.h - header for the slippy step class

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CSlippyStep_h_
#define _CSlippyStep_h_

//header files---------
#include "CTowerStep.h"
//---------------------

//defines--------------
//---------------------

//external definitions --
//-----------------------

class CSlippyStep : public CTowerStep
{
	private:
//-----private variables-----------
		CVector forceVector;
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		/*	Constructor with model parameter, tower position and direction nebulus will 
			slip off the step */
		CSlippyStep(CModel * aModel, int towerPosition, bool slippyStepLeft); 
		~CSlippyStep(); // destructor
		// collision response
		void collisionResponse(	CSprite * collidedWith,
								float perCentOfMoveComplete, 
								CVector * polyintersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc);
		// reinitialise to starting state (empty as sprites characteristics never change)
		void reinitialise( void){}

		// overloaded operators

//-----------------------------------------------------------
}; // end class CSlippyStep

#endif // ifndef _CSlippyStep_h_ 