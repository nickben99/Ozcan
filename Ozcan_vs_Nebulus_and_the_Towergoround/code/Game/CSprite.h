//CSprite.h - header for the sprite class

//system includes------------
#include "Rendering/OpenGLInclude.h"
#include <vector>		// include this to use the vector class
//---------------------------

#ifndef _CSprite_h_
#define _CSprite_h_

//header files---------
#include "Math/CVector.h"
#include "Math/CMatrix.h"
#include "Rendering/CModel.h"
#include "Rendering/CParticleSystem.h"
#include "Audio/SoundsEnum.h"
//---------------------

// forward declarations
class COctree;
//---------------------

//defines--------------
enum collisionTypes // different approximations of geometry
{
	AXIS_ALIGNED_BOUNDING_RECTANGLE,
	ORIENTED_BOUNDING_RECTANGLE,
	BOUNDING_SPHERE,
	BOUNDING_ELIPSOID,
	BOUNDING_CYLINDAR,
	BOUNDING_CAPSULE,
	TRIANGLES,
	RAY
};                
enum spriteTypes // different sprites
{
	SNEBULUS, 
	SSTEP, 
	SLIFT_BOTTOM, 
	SCOLLAPSING_STEP, 
	STUNNEL, 
	SSLIPPY_STEP, 
	STOWER_TOP_OR_BASE, 
	SPOWER_UP, 
	SFIRE_BALL, 
	SUP_DOWN_ENEMY, 
	SSIDE_TO_SIDE_ENEMY, 
	SBUFFER_STEP, 
	SBOUNCING_SIDE_TO_SIDE_ENEMY, 
	SFLY_ACROSS_ENEMY, 
	SEXIT_SCREEN, 
	UNSPECIFIED
};
enum dynamicTypes // whether sprites move or are part of the environment
{
	MOVING, STATIC_ENVIRONMENT, MOVING_ENVIRONMENT
};
//---------------------

class CSprite
{
	protected:
//-----private variables-----------
		struct ReplayData
		{
			int frame;
			CVector position;
			CQuaternion orientation;
			CVector scale;
			bool active;
			bool drawThisFrame;
			GLfloat currentAnimationFrame;
			bool particleSystemActive;
			int particleSystemFrame;
			bool transparency;
			SOUNDS soundToPlay;
		};
		ReplayData* mpReplayData;

		virtual void saveReplayUpdate(ReplayData&, bool);
//---------------------------------

	public:
//----public variables--------------
		CVector velocity, // the sprites movement delta
				oldPosition, // the sprites previous position
				position,	// the sprites position
				forces,		/*	forces acting on the sprite resulting in acceleration or 
								deceleration */
				elipsoidRadiusVector, // for elipsoid bounding spheres
				gravitation, /* different gravitational acceleration may be applied to
								different sprites */
				scale,			// for specific scaling along cardinal axis, 1 = full size
				boundingBox,	// the models bounding box 
				scaledBoundingBox, // current bounding box including scaling
				scaledHalfBoundingBox, // the current half bounding box including scaling
				scaledElipsoidRadiusVector; // current elipsoid radius vector inc scaling

		CMatrix rotation, // the sprites rotation
				finalMatrix; // includes translation, rotation and scaling

		GLfloat mass,	// mass of the sprite in kg
				surfaceFrictionConstant, // physical values for sprites which move
				airFrictionConstant, 
				forwardThrust;

		CModel * model; //model which represents this sprite
		GLfloat currentFrame,  // the current frame of the model representing this sprite
				boundingSphereRadius; /*	the bounding sphere of the current frame of the
											model representing this sprite */	
		bool	loopingAnimation, // whether the animation plays once or repeatedly loops
				transparency,	/*	whether the sprites model includes transparent elements, 
									required so transparent models can be drawn last */
				active; // whether the sprite is currently active

		GLint	framesPerSecond; /*	how many frames per second.  The higher this amount the 
									quicker the animation will run */
		collisionTypes collisionType;	/*	defines how the object geometry is approximated 
									for collision detection purposes */
		dynamicTypes dynamicProperty;	/*	does this sprite move?, is it static environment or 
										moving environment? */ 
		spriteTypes	spriteType;		// what type of sprite this is
		SOUNDS soundPlayedThisFrame;
		
		std::vector<COctree*> endNodesOccupying; // the end nodes the sprite is occupying

// specifically for steps ----------
		float frictionalCoefficient;
//----------------------------------

// specifically for collapsing steps----
	/*	the radius a sphere would need to be to compleatly 
		encapsulate the collapsing steps model */
	float boundingBoxRadius;
// --------------------------------------

// specifically for capsules and cylindars ----
		float height;
// --------------------------------------------

// specifically for Nebulus -------------------
		CVector drawScale; /*	sometimes Nebulus's scale and the scale he is 
								drawn at will differ */
		float	jumpThrust, 
				topGroundSpeedFramePerSecond,
				midAirGroundPlaneMaxSpeed; // maximum x, z velocity when in mid air

		// whether nebulus is using a power-up, so two cannot be used at the same time
		bool currentlyUsingPowerUp;
// --------------------------------------------

// specifically for enemies -------------------
		GLuint killValue; // the amount the enemy is worth in terms of points if killed by nebulus' fireball
// --------------------------------------------

//----public methods----------------------------------------
		CSprite(); // constructor
		CSprite(CModel * aModel); // Constructor with model parameter
		void applyForce(CVector * force); // apply a force to the sprite
		void applyForce(const CVector& force); // apply a force to the sprite

		// virtual methods
		virtual ~CSprite(); // destructor
		virtual void initialise(); // initialisation
		virtual void keyInput(){}// process key input for the sprite
		// move this sprite
		virtual void move(int timeChange, GLfloat perCentOfSecond); 
		virtual void calculateFinalMatrix(); // calculate the final matrix
		virtual void draw(int timeChange); // draw this sprite
		// collision response (overloaded methods)
		virtual void collisionResponse(	CSprite*,
										float, 
										CVector*, 
										CVector*, 
										CVector*) {} 
		virtual void collisionResponse(	CSprite*) {}
		virtual void reinitialise( void); // reinitialise sprite to starting state

		void saveReplayUpdate(int frame);
		virtual void ReplayInit();

// specifically for Nebulus ----------------------------
		// deal with input control to Nebulus
		virtual void keyInput(const CVector&, const CVector&){}
//------------------------------------------------------

		// overloaded operators

//-----------------------------------------------------------
}; // end class CSprite

#endif // ifndef _CSprite_h_ 