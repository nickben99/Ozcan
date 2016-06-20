//CTowerStep.h - header for the tower step class (includes CBufferStep)

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CTowerStep_h_
#define _CTowerStep_h_

//header files---------
#include "CTowerSprite.h"
//---------------------

//defines--------------
//---------------------

class CTowerStep : public CTowerSprite
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		CTowerStep(); // constructor
		CTowerStep(CModel * aModel); // overloaded constructor
		// Constructor with model parameter and tower position
		CTowerStep(CModel * aModel, int towerPosition); 
		~CTowerStep(); // destructor

		// virtual methods
		virtual void initialise(int towerPosition); // initialisation
		// reset the sprite to starting state
		virtual void reinitialise();

		// overloaded operators

//-----------------------------------------------------------
}; // end class CTowerStep

#endif // ifndef _CTowerStep_h_ 


#ifndef _CBufferStep_h_
#define _CBufferStep_h_

//header files---------
#include "CTowerSprite.h"
//---------------------

//defines--------------
//---------------------

class CBufferStep : public CTowerStep
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables--------------
//----------------------------------

//----public methods----------------------------------------
		CBufferStep(){} // constructor
		CBufferStep(CModel*){} // overloaded constructor
		// Constructor with model parameter
		CBufferStep(CModel * aModel, int towerPosition)
			: CTowerStep(aModel, towerPosition) // initialise super class
		{
			spriteType = SBUFFER_STEP; // set sprite type
		}
		
		~CBufferStep() {CTowerStep::~CTowerStep();} // destructor
		// reinitialise to starting state (empty as sprites characteristics never change)
		void reinitialise( void){}

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CBufferStep

#endif // ifndef _CBufferStep_h_ 