//CParticleSystem.cpp, the camera class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CParticleSystem.h"
#include "Math/CMatrix.h"
#include "Game/CLevel.h"
#include "Game/Globals.h"
#include "Game/ReplayManager.h"
#include "Game/GameTime.h"
#include <System/Interfaces/SystemInterface.h>
//-------------------------

//--- external variables --------
//-------------------------------

extern GLuint TextureLoad(const char *filename, bool alpha, GLenum minfilter, GLenum magfilter, GLenum wrap);

//---static variable declerations ---
int CParticleSystem::particleTexture = 0;
#ifdef USE_SHADERS
MeshBuffer CParticleSystem::triangleMesh;
#endif
//-----------------------------------

// constructor
CParticleSystem::CParticleSystem()
{
	initialise();
}

void CParticleSystem::initialise()
{
	systemActive = false; // no particles are active
	gravity = CVector(0.0f, -9.81f, 0.0f);
	numFramesActive = -1;
}

// destructor
CParticleSystem::~CParticleSystem()
{
	deleteParticleSystem();

#ifdef USE_SHADERS
	triangleMesh.Delete();
#endif
}

// set up the particle system texture etc
int CParticleSystem::initialiseParticleSystem() 
{	
	// if the particle texture has not been loaded yet
	if (particleTexture <= 0)
	{ 
		char buffer[256];
        SPRINTF(buffer, "%simages/particle.bmp", GetDirectoryPath()); // create file name with path

		particleTexture = // bind the background texture
			TextureLoad(buffer, GL_FALSE, GL_LINEAR, GL_LINEAR, GL_REPEAT);
	}

	// if their were problems loading the particleTexture
	if (particleTexture <= 0){ 
		return(0);} // unsuccesful load

#ifdef USE_SHADERS
	GLfloat vertexPositionArray[] = {
		-1.2f, 1.2f, 0.0f,
		-1.2f, -1.2f, 0.0f,
		1.2f, -1.2f, 0.0f
	};

	triangleMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));

	float vertexTexCoordsArray[] = 
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	triangleMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
	triangleMesh.SetTexture(particleTexture); 
#endif

	return(1); // success
}

// delete the particle systems texture etc
void CParticleSystem::deleteParticleSystem( void)
{
	// if the particle texture has not yet been deleted
	if (particleTexture > 0)
	{
		// delete the particle texture
		deleteTexture(particleTexture);
		particleTexture = 0; // reset to zero
	}
}

void CParticleSystem::saveReplayUpdate(bool& isActive, int& particleSystemFrame, bool isFirstFrame, CVector* position)
{
	if (ReplayManager::instance()->IsRecording())
	{
		isActive = this->systemActive;
		particleSystemFrame = numFramesActive;
	}
	else // replaying
	{
		if (isActive && (isFirstFrame || 0 == particleSystemFrame))
		{
			resetAllParticles(position, particleSystemFrame);
		}
	}
}

// initialise the particle system
void CParticleSystem::resetAllParticles(CVector *startingPosition, int frame) 
{
	startPos = *startingPosition; // set the starting position of the explosion
	systemActive = true; // the system is now active
	numFramesActive = 0; //  how long we've been active

	// initialise all the particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particle &aParticle = particles[i]; // referance variable

		aParticle.active = true; // the particle is now active
		aParticle.life = 1.0f; /*	start with full life (will be reduced 
										by the fade value) */
		/*	results in a random number between 2 and 5 which is 
			0.20 and 0.50 seconds */
        aParticle.fade = defines::FRandom(20, 50)*0.1f;
		/*	set colour, which will have 'life' as the alpha value.
			colour will be somewhere between orange and red */	
		aParticle.r = 1.0f; //FRANDOM(0, 100)*0.01f;  
		aParticle.g = defines::FRandom(0, 100)*0.01f;
		aParticle.b = defines::FRandom(0, 100)*0.01f;
		// set starting position
		aParticle.position = *startingPosition;
		// set the velocity to a random direction
		aParticle.velocity = CVector(defines::FRandom(-26, 26), defines::FRandom(-26, 26), defines::FRandom(-26, 26));
	}

	while (numFramesActive < frame)
	{
		move(GameTime::Instance().GetCurrentFixedTimeDeltaSeconds());
		++numFramesActive;
	}
}

// move all the particles
void CParticleSystem::move(GLfloat perCentOfSecond)
{
	if (!systemActive)
	{
		numFramesActive = -1;
		return;
	}

	if (!GameTime::Instance().IsPaused())
	{
		++numFramesActive;
	}
	systemActive = false; /*	set to inactive here, but if any particles have life left 
							this will be set to 1 */ 
	CVector forces			= (gravity*MASS), // only force acting on particle is gravity
			acceleration	= (forces/MASS)*perCentOfSecond; // set acceleration for frame

	// move all the particles by integrating acceleration and velocity
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particle &aParticle = particles[i]; // referance variable

		if (aParticle.life > 0.0f)
		{
			// if at least one particle has life left, set the system to active
			systemActive = true;
			// take away life based on the fade/second value
			aParticle.life -= (aParticle.fade * perCentOfSecond);
			// add acceleration to velocity
			aParticle.velocity += &( acceleration );
			// add velocity to position
			aParticle.position += (aParticle.velocity * perCentOfSecond); 
		}
	}
}

// draw all the particles
void CParticleSystem::draw( void)
{
	if (!systemActive)
	{
		return;
	}

	Globals::Instance().gl.GLEnable(GL_BLEND);	// enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); /*	use GL_ONE type blending rather than 
											GL_ONE_MINUS_SRC_ALPHA as used in the 
											rest of the programme */
#ifdef USE_SHADERS
	Globals::Instance().gl.GLEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, particleTexture); // set the texture to be used
#endif	
	Globals::Instance().gl.GLDisable(GL_LIGHTING);
	Globals::Instance().gl.GLDisable(GL_DEPTH_TEST); /*	disable depth testing (essential for rendering 
									particles correctly) */
	// draw all the particles
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		// referance variable = speed and readability
		particle &aParticle = particles[i];
		// draw only if the particle has any life left
		if (aParticle.life > 0.0f)
		{
			// use the particles rgb value and life as the alpha value
#ifdef USE_SHADERS
			triangleMesh.SetColor(CVector4(aParticle.r, aParticle.g, aParticle.b, aParticle.life));
#else
			glColor4f(aParticle.r, aParticle.g, aParticle.b, aParticle.life);
#endif

			CMatrix translate;
			CVector camUp(0.0f, 1.0f, 0.0f);
			// work out the billboard vector values
			CVector look =	(Globals::Instance().currentLevel.theScene.theCamera->GetGameCameraPosition() - &aParticle.position)/
							(Globals::Instance().currentLevel.theScene.theCamera->GetGameCameraPosition() - &aParticle.position).getMagnitude(), 
					right =	camUp.crossProduct(&look), 
					up =	look.crossProduct(&right);

			// set-up the billboard matrix
			translate.createBillboardMatrix(&right, &up, &look, &aParticle.position);

			Globals::Instance().modelMatrixStack.PushMatrix(); // save previous matix
				Globals::Instance().modelMatrixStack.MultMatrix(translate);
#ifdef USE_SHADERS
				triangleMesh.Draw();
#else
				glBegin(GL_TRIANGLE_STRIP); // draw particle triangle
					glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.2f, 1.2f, 0.0f); // bottom left
					glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.2f, -1.2f, 0.0f); // bottom right
					glTexCoord2f(1.0f, 1.0f); glVertex3f(1.2f, -1.2f, 0.0f); // top left 
				glEnd();
#endif
			Globals::Instance().modelMatrixStack.PopMatrix(); // go back to previous matrix
		}
	}
	Globals::Instance().gl.GLEnable(GL_LIGHTING); // re enable lighting
	Globals::Instance().gl.GLEnable(GL_DEPTH_TEST);
}