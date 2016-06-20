//CLift.h - header for the tower step class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CLift_h_
#define _CLift_h_

//header files---------
#include "CTowerStep.h"
//---------------------

//defines--------------
//---------------------

class CLift : public CTowerStep
{
	private:
//-----private variables-----------
		int liftTopYHeight, // actual height of the top of the lift
			distanceToLiftTop; /*	difference in height between the 
									top and bottom of the lift */
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CLift(CModel * aModel, int towerPosition, char LiftTopIdentifier); 
		~CLift(); // destructor
		// colliion response
		void collisionResponse(	CSprite * collidedWith,
								float perCentOfMoveComplete, 
								CVector * polyintersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * velocity);
		// reinitialise to starting state (empty as sprites characteristics never change)
		void reinitialise( void){}

		// overloaded operators

//-----------------------------------------------------------
}; // end class CLift

#endif // ifndef _CLift_h_ 