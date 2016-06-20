//CNebulus.h - header for the Nebulus class

//system includes------------
#include <windows.h>	// header file for windows
#include <gl\gl.h>		// header file for the OpenGL32 library
#include <gl\glu.h>		// header file for the GLu32 library
//---------------------------

#ifndef _CNebulus_h_
#define _CNebulus_h_

//header files---------
#include "CTowerStep.h"
#include "CTunnel.h"
#include "Math/CQuaternion.h"
#include "Audio/SoundsEnum.h" // for playing sounds
//---------------------

//defines--------------
#define NEBULUS_FORWARD_THRUST 7.0f
#define NEBULUS_JUMP_THRUST 7.0f
#define NEBULUS_MID_AIR_GROUND_PLANE_MAX_SPEED 2.5f
#define NEBULUS_SURFACE_FRICTION_CONSTANT 1.52f
#define NEBULUS_AIR_FRICTION_CONSTANT 0.52f
#define NEBULUS_STARTING_LIVES 3
// animation parameters
#define WALK_START					0.0f   // the frame time from the nebulus ASCII files
#define WALK_END					40.0f
#define WALKING_TO_STANDING_START	40.0f
#define WALKING_TO_STANDING_END		50.0f
#define STAND_START					50.0f
#define STAND_END					70.0f
#define STANDING_TO_WALKING_START	70.0f
#define STANDING_TO_WALKING_END		80.0f

enum animationStates // different animation states the nebulus model can be in
{
	WALKING,
	STANDING,
	STANDING_TO_WALKING, 
	WALKING_TO_STANDING
};

struct animation
{
	int currentAnimationState, // the current nebulus animation running (from states above)
		nextAnimationState; /*	the animation to go to after this animation finishes, 
								-1 indicates to keep playing the current animation */
	bool looping; /* if this animation has 'nextAnimationState' set to -1 and looping is 
					false, after the animation has run the final frame will just be 
					displayed */

	float startFrame, endFrame; // starting and ending frame of the current animation
};
//---------------------

//external definitions-------
//---------------------------

// forward declarations ----------
//--------------------------------

class CNebulus : public CTowerStep
{
	private:
//-----private variables-----------
		static CNebulus *nebulus; // singleton nebulus
		animation currentAnimInfo;
		CTunnel* mpTunnel;
//---------------------------------

//-----private methods-------------
		CNebulus(CModel * aModel, int towerPosition, int livesLeft); // constructor
		void initialise(int livesLeft);
		// get the starting and ending frames for the animation required
		void getAnimationStartingAndEndingFrame(int animation, float &startFrame, 
																float &endFrame);
		void ShootOut();
//---------------------------------

	protected:
		void saveReplayUpdate(ReplayData& data, bool);

	public:
//----public variables--------------
		CVector userCtrlDelta;
		CVector userCtrlRotDelta;
		bool	currentlyInSurfaceContact, /*	records whether nebulus is currently in contact
												with a surface or not */
				onJumpableSurface; // this surface can be jumped off
		bool	wasShotOut;

		float	approximateTopGroundSpeed, 
				// top speed, used if a 180 degree turn is required
				angularTopSpd;

		int		lives; // how many lives Nebulus has remaining
		GLuint	score, // nebulus' current score
				levelCompleted; // a flag to indicate Nebulus has completed the level
//----------------------------------

//----public methods----------------------------------------
		// returns the Nebulus singleton instance
		static CNebulus* instance(CModel * aModel, int towerPosition, int livesLeft); 
		static CNebulus* getExistingInstance(); /*	returns a pointer only if the static 
													instance has already been initiated */			
		static void destroyInstance();	// destroy nebulus instance
		~CNebulus(); // destructor
		// deal with input control to Nebulus
		void keyInput(const CVector& forward, const CVector& right); 
		void move(int timeChange, GLfloat perCentOfSecond); // move nebulus
		virtual void calculateFinalMatrix(); // calculate the final matrix
		// collision response
		void collisionResponse(CSprite * collidedWith, 
								float perCentOfMoveComplete,
								CVector * polyintersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc);	/*	response to hitting a 
														perticular sprite */		
		// reset Nebulus back to starting state
		void reinitialise( void);

		CTunnel* GetTunnel(); // return the tunnel nebulus is currently inside

		// overloaded operators

//-----------------------------------------------------------
}; // end class CNebulus

#endif // ifndef _CNebulus_h_ 