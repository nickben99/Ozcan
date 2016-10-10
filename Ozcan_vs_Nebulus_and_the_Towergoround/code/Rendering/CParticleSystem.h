//CParticleSystem.h - header for the particle system class

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CParticleSystem_h_
#define _CParticleSystem_h_

//header files---------
#include "Math/CVector.h"
#include "FileReading/texture.h"
#include "Game/defines.h"
#include "Rendering/MeshBuffer.h"
//---------------------

//defines--------------
struct particle
{
	int		active;					// active (yes/no)
	float	life,					// particle life (starts at 1.0f, dead when 0.0f)
			fade,					/*	particle fade in seconds (e.g 0.5 would be dead 
										in 2 seconds, 2 would be dead in 0.5 seconds) */
			r,						// red colour element
			g,						// green colour element
			b;						// blue colour element

	CVector position,				// x,y,z position
			velocity;				// x,y,z velocity
};
#define MAX_PARTICLES	500	// the number of particles in the particle system
#define MASS			0.01f // mass of a single particle 0.01 kg's
//---------------------

//forward declerations----
//------------------------

class CParticleSystem
{
	private:
//-----private variables-----------
		particle particles[MAX_PARTICLES];
		CVector gravity, // gravitational force on the particles
				startPos;
		int numFramesActive;

//------static variables ----------
		static int particleTexture; // the particle texture shared between all the instances of this class
#ifdef USE_SHADERS
		static MeshBuffer triangleMesh; // shared between all the instances of this class
#endif
//---------------------------------
//---------------------------------

//-----private methods-------------
//---------------------------------

	public:
//----public variables--------------
		bool systemActive; // if any particles have any life left the system is active
//----------------------------------

//----public methods----------------------------------------
		CParticleSystem(); // constructor
		~CParticleSystem(); // destructor 
		// delete the particle systems texture etc
		void deleteParticleSystem( void);
		void resetAllParticles(CVector *startingPosition, int frame = 0); // initialise the particle system
		// move all the particles
		void move(GLfloat perCentOfSecond);
		// draw all the particles
		void draw( void);

		void saveReplayUpdate(bool& systemActive, int& systemFrame, bool isFirstFrame, CVector* position);

		// virtual methods

		// static methods
		// set up the particle system texture etc
		static int initialiseParticleSystem();

		void initialise();

		// overloaded operators

//-----------------------------------------------------------
}; // end class CParticleSystem

#endif // ifndef _CParticleSystem_h_ 