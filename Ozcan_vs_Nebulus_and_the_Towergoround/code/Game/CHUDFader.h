//CHUD.h - header for the Head Up Display (HUD) class

// system includes ----------
#include <stdio.h>			// header file for standard input/output
#include "Rendering/OpenGLInclude.h"
// --------------------------

#ifndef _CHUDFader_h_
#define _CHUDFader_h_

//header files-------------------
//-------------------------------

// defines ----------------------
// ------------------------------

class CHUDFader
{
	private:
//-----private variables-----------
//---------------------------------

//-----private methods--------------------		
		float alpha;

		enum state
		{
			eFadingIn = 0, 
			eFadingOut, 
			eStateMax
		} currentState, nextState;
//----------------------------------------

	public:
//----public variables-------------
//---------------------------------

//----public methods----------------------------------------
		CHUDFader( void);
		~CHUDFader();

		void update(float delta); // update any quantities displayed on the HUD
		void draw( void); // draw the HUD to the screen

		void FadeIn();
		void FadeOut();
		void FadeInThenOut();
		void FadeOutThenIn();
//-----------------------------------------------------------
}; // end class CHUD

#endif // ifndef _CHUDFader_h_ 