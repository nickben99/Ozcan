//CNebulus.cpp, a nebulus class

// system includes --------
// #include <math.h>
//-------------------------

// header files -----------
#include "CNebulus.h"
#include "ReplayManager.h"

#include "Game/Globals.h"
//-------------------------

// static variables------------
CNebulus* CNebulus::nebulus = 0; // initialise pointer
//-----------------------------

CNebulus* CNebulus::instance (CModel * aModel, int towerPosition, int livesLeft) 
{
	if (!nebulus){  // is it the first call?
		nebulus = new CNebulus(aModel, towerPosition, livesLeft);} // create sole instance

    return nebulus; // address of sole instance
}

// returns a pointer only if the singleton has already been initiated
CNebulus* CNebulus::getExistingInstance()
{
	if (nebulus){ // if already initiated
		return nebulus;} // return pointer
	return 0; // return null as it has not been initiated
}

// constructor
CNebulus::CNebulus(CModel * aModel, int towerPosition, int livesLeft)
	: CTowerStep(aModel, towerPosition)
	, mpTunnel(0)
	, levelCompleted(false)
{
	initialise(livesLeft);
}

// initialise sprite
void CNebulus::initialise(int livesLeft)
{
	mpTunnel = 0;
	// rotate nebulus so he begins facing to the right
	CMatrix mat;
	mat.createYRotationMatrix(degToRad(90));

	rotation *= &mat;
	
	approximateTopGroundSpeed = 3.40f; // set usual top walking speed
	topGroundSpeedFramePerSecond = 88.0f; // frames per second for top walking speed
	angularTopSpd = 4.0f*PI, // top speed, used if a 180 degree turn is required
	collisionType = BOUNDING_ELIPSOID; /* nebulus is represented by a bounding ellipsoid for
										collision detection */	
	spriteType = SNEBULUS; // set sprite type
	dynamicProperty = MOVING; // nebulus is a moving sprite
	scale = CVector(1.0f, 1.0f, 1.0f); // unscaled
	currentlyInSurfaceContact = false; // not currently in contact with a surface
	onJumpableSurface = false; // not currently in contact with a surface which can be jumped off
	wasShotOut = false;
	
	// set Nebulus's radius vector
	elipsoidRadiusVector = CVector(boundingSphereRadius*0.5f, boundingSphereRadius*0.7f, 
									boundingSphereRadius*0.5f); 

	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale; // (must be done after setting up elipsoidRadiusVector above)

	drawScale = scale; /*	drawScale and scale are set to the same here.  However sometimes 
							Nebulus may be drawn at a perticular scale while its normal scale 
							which would be used in collision detection will be different.  This 
							is used in the grow power-up as lerping the grow can cause 
							collision anomolies.  So the scale is instantly changed while 
							Nebulus appears to grow */

	forwardThrust = NEBULUS_FORWARD_THRUST; // forward acceleration per second 
	jumpThrust = NEBULUS_JUMP_THRUST; // jump velocity setting
	midAirGroundPlaneMaxSpeed = NEBULUS_MID_AIR_GROUND_PLANE_MAX_SPEED; // max ground plane velocity when in mid air (per second)
	surfaceFrictionConstant = NEBULUS_SURFACE_FRICTION_CONSTANT;
	airFrictionConstant = NEBULUS_AIR_FRICTION_CONSTANT;

	currentlyUsingPowerUp = false; // initially no power-up is being used

	lives = livesLeft; // nebulus starts with three lives
	score = 0; // nebulus starts with a score of zero

	currentAnimInfo.currentAnimationState = WALKING; // start in walking animation
	currentAnimInfo.nextAnimationState = -1; // no next animation
	currentAnimInfo.looping = false; // false that the animation is looping
	currentAnimInfo.startFrame = WALK_START; // start and end frames
	currentAnimInfo.endFrame = WALK_END;

	// so sprite will bedrawn in correct position after reinitialising
	oldPosition = position;
	calculateFinalMatrix();
}

// reset Nebulus back to starting state
void CNebulus::reinitialise( void)
{
	CTowerStep::reinitialise();
	initialise(lives); // decrement lives every time reinitialised 
}

// destroy nebulus instance (static method)
void CNebulus::destroyInstance()
{
	if (nebulus) // if nebulus has been initiated
	{
		delete nebulus; // delete nebulus
		nebulus = 0; // set pointer to null
	}
}

// destructor
CNebulus::~CNebulus()
{
}

// deal with input control to Nebulus
void CNebulus::keyInput(const CVector& forward, const CVector& right)
{
#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(forward);
	ReplayManager::instance()->VectorResult(right);
	ReplayManager::instance()->VectorResult(userCtrlDelta);
#endif

	// set nebulus's direction vector
	if(Globals::Instance().keys.GetKeyStatus(eKeyCode_A, true)){
		userCtrlDelta.setX(-1.0f);} 
	if(Globals::Instance().keys.GetKeyStatus(eKeyCode_D, true)){
		userCtrlDelta.setX(1.0f);}
	if(Globals::Instance().keys.GetKeyStatus(eKeyCode_W, true)){
		userCtrlDelta.setZ(1.0f);} 
	if(Globals::Instance().keys.GetKeyStatus(eKeyCode_S, true)){
		userCtrlDelta.setZ(-1.0f);}

#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(userCtrlDelta);
#endif

	// if nebulus has moved this frame (through button presses above)
	if (!(userCtrlDelta.fuzzyEquals(CVector())))
	{	// work out direction nebulus should move based on direction the camera is pointing
		float	x = (userCtrlDelta.x*right.x)+(userCtrlDelta.z*forward.x),
				z = (userCtrlDelta.x*right.z)+(userCtrlDelta.z*forward.z);

#ifdef DETERMINISTIC_REPLAY_DEBUG
		ReplayManager::instance()->FloatResult(x);
		ReplayManager::instance()->FloatResult(z);
		ReplayManager::instance()->FloatResult(forwardThrust);	
#endif

		// set the nebulus movement variables
		userCtrlDelta.x = x;
		userCtrlDelta.z = z;
		// set the magnitude of the movement
		userCtrlDelta.setMagnitude(forwardThrust);	
	}

#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(userCtrlDelta);
#endif

	// see if Nebulus should jump
	if(Globals::Instance().keys.GetKeyStatus(eKeyCode_M, true) && onJumpableSurface)
	{
		velocity.setY(jumpThrust);
	}
}

// move the sprite
void CNebulus::move(int timeChange, GLfloat perCentOfSecond)
{
	mpTunnel = 0;
	soundPlayedThisFrame = SOUNDS_COUNT;

	applyForce((gravitation * mass));	//gravitational force is as F = m * g. (mass times the gravitational acceleration)
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//	ReplayManager::instance()->VectorResult(gravitation);
//	ReplayManager::instance()->FloatResult(mass);
//	ReplayManager::instance()->VectorResult(forces);
//#endif

	/* apply air friction force (causes terminal velocity so nebulus will not
								continue to accelerate downwards, velocity will eventually
								level off) */
	applyForce((-velocity * airFrictionConstant)); // airFrictionConstant originally 0.02f
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//	ReplayManager::instance()->FloatResult(airFrictionConstant);
//	ReplayManager::instance()->VectorResult(velocity);
//	ReplayManager::instance()->VectorResult(forces);
//#endif

	/*	surface friction brings nebulus to a hault if 
		no forces are being applied to it (could get frictional 
		value of surface in contact with)*/
	if (currentlyInSurfaceContact) {
		applyForce((-velocity * surfaceFrictionConstant));} // surface friction
	else // not in surface contact 
	{	// get ground plane velocity
		CVector upVector(0.0f, 1.0f, 0.0f),
				virticalVeloc = (upVector * (velocity.dotProduct(&upVector))),
				groundPlaneVeloc = velocity - &virticalVeloc;
		// restrict side to side movement when in mid air with ground plane air friction
		applyForce((-groundPlaneVeloc * surfaceFrictionConstant));
	}
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//	ReplayManager::instance()->FloatResult(surfaceFrictionConstant);
//	ReplayManager::instance()->VectorResult(forces);
//#endif

	// apply the user ctrl force
	applyForce(&userCtrlDelta);

//#ifdef DETERMINISTIC_REPLAY_DEBUG
//	ReplayManager::instance()->VectorResult(forces);
//#endif
	
	// make copy of userCtrlDelta and just consider the ground plane movement, not the jump action
	CVector groundPlaneCtrlDelta = userCtrlDelta;
	groundPlaneCtrlDelta.y = 0.0f;
	// if nebulus has moved under his own force this frame (not including jumping)
	if (!(groundPlaneCtrlDelta.fuzzyEquals(CVector())))
	{
		CVector nebulusDirectionNormal(0.0f, 0.0f, 1.0f);
		// work out Nebulus's current rotation vector
		nebulusDirectionNormal = rotation.multiplyRotateVector(&nebulusDirectionNormal);
		/*	get angle between nebulus's current forward vector and the direction of the vector 
			indicating in which direction Nebulus is moving in */
		float angleBetween = nebulusDirectionNormal.getAngleBetween(&userCtrlDelta);
		// work out the speed for this frame (higher speed as angle is closer to 180 degrees)
		float	speedThisFrm = (angleBetween/PI)*angularTopSpd;
		// get amount to move this frame
		float	movementThisFrame = perCentOfSecond*speedThisFrm;
		// if movement will go beyond the angle restrict it to the angle
		if (movementThisFrame > angleBetween){
			movementThisFrame = angleBetween;}

		// get cross product of the two vectors and normalise
		CVector cross;
		cross.crossProduct(&nebulusDirectionNormal, &userCtrlDelta);
		cross.normalise();
		/*	if the cross product could not be derived, which could happen if the vectors are 
			at an angle of 180 degrees from one and other and thus parallel and no plane can 
			be derived (also potentially if vectors are in the same direction, but no 
			movement would be required then anyway) */
		if (cross.fuzzyEquals(CVector())){
			cross = CVector(0.0f, 1.0f, 0.0f);} // set axis

		// create rotation matrix from angle and axis
		CMatrix move;
		move.createArbitraryAxisRotation(movementThisFrame, cross);
		rotation *= &move; // add the rotation onto Nebulus's current rotation
	}// end if groundPlaneCtrlDelta was not zero

	// record old position (used in collision detection and response)
	oldPosition = position; 
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//	ReplayManager::instance()->VectorResult(oldPosition);
//#endif
	CVector acceleration = (forces / mass); // determine acceleration
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//	ReplayManager::instance()->VectorResult(acceleration);
//#endif

	velocity += ( acceleration * perCentOfSecond);	// add acceleration to velocity
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//	ReplayManager::instance()->VectorResult(velocity);
//#endif
	
	position += (velocity * perCentOfSecond); // add velocity to position

	if (wasShotOut)
	{		
		if (position.Get2D().getMagnitudeSquared() < oldPosition.Get2D().getMagnitudeSquared()) // are we moving closer to the tower
		{
			wasShotOut = false; // we are recovering from the shoot out, moving back into the tower
		}
	}

	if (ReplayManager::instance()->IsRecording())
	{
		if (wasShotOut)
		{
			ReplayManager::instance()->SetEndReplayStartFrame(); // tell the replay system that this would be a good start frame for a replay
		}
		else if (!onJumpableSurface)
		{
			if (position.y < oldPosition.y) // if we're moving down
			{
				ReplayManager::instance()->SetEndReplayStartFrame(); // tell the replay system that this would be a good start frame for a replay
			}
		}
		else
		{
			ReplayManager::instance()->ClearEndReplayStartFrame();
		}
	}

#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(position);
#endif

	/*	determines how fast or slow the nebulus animation runs, dependant on how fast 
		nebulus is moving (the 40.0f imposes a minimum frame rate) */
	framesPerSecond = (GLint)( 40.0f+(velocity.getMagnitude()/approximateTopGroundSpeed)*(topGroundSpeedFramePerSecond-40.0f) );

	// work out the virtical velocity and the ground plane velocity
	CVector upVector(0.0f, 1.0f, 0.0f),
			virticalVeloc = (upVector * (velocity.dotProduct(&upVector))),
			groundPlaneVeloc = velocity - &virticalVeloc;

	/*	the following section of code determines which of the nebulus animations 
		should be playing at the current time */
	// if should be in walking state
	if (groundPlaneVeloc.getMagnitude() > 0.9f || 
		virticalVeloc.getMagnitude() > 0.6f)
	{
		// if not currently in 'WALKING' state
		if (WALKING != currentAnimInfo.currentAnimationState)
		{
			switch (currentAnimInfo.currentAnimationState)
			{
				case STANDING:
				case WALKING_TO_STANDING:
					// go to 'STANDING_TO_WALKING' and subsequently 'WALKING'
					currentAnimInfo.nextAnimationState = STANDING_TO_WALKING; 
					currentAnimInfo.looping = false; // just finish off playing
					break;
				case STANDING_TO_WALKING:
					currentAnimInfo.nextAnimationState = WALKING; // go to 'WALKING' 
					currentAnimInfo.looping = false; // just finish off playing
					break;
				default: break;
			} 
		}
		else{ // in correct 'WALKING' state
			currentAnimInfo.looping = true;} // keep playing
	}
	else // should be in standing state
	{
		// if not currently in 'STANDING' state
		if (STANDING != currentAnimInfo.currentAnimationState)
		{
			switch (currentAnimInfo.currentAnimationState)
			{
				case STANDING_TO_WALKING:
				case WALKING:
					// go to 'WALKING_TO_STANDING' and subsequently 'STANDING'
					currentAnimInfo.nextAnimationState = WALKING_TO_STANDING; 
					currentAnimInfo.looping = false; // just finish off playing
					break;
				case WALKING_TO_STANDING:
					currentAnimInfo.nextAnimationState = STANDING; // go to 'STANDING' 
					currentAnimInfo.looping = false; // just finish off playing
					break;
				default: break;
			} 
		}
		else{ // in correct 'STANDING' state
			currentAnimInfo.looping = true;} // keep playing
	}

	userCtrlDelta.reset();
	userCtrlRotDelta.reset();
	currentlyInSurfaceContact = false; /*	reset to false (may become true 
											during collision detection and response) */
	onJumpableSurface = false;			/*	reset to false (may become true 
											during collision detection and response) */
	// set gravitational acceleration (may change during collision detection and response)
	gravitation = CVector(0.0f, -9.81f, 0.0f);
	forces.reset(); /*	reset forces (new forces will be added during collision response and 
						this move method in the next cycle) */
	
	// if the sprite has an associated model
	if (model) 
	{
		/*	this following section of code advances the current frame and ensures the 
			correct animation is playing or will begin playing */
		float amtOver;
		// advance the animation
		model->advanceFrame(timeChange, currentFrame, framesPerSecond, currentAnimInfo.looping, 
										(int)currentAnimInfo.startFrame, (int)currentAnimInfo.endFrame, &amtOver);
		/*	if the end frame of the current animation has been reached and this is 
			not a looping animation */
		if (currentFrame == currentAnimInfo.endFrame && !currentAnimInfo.looping)
		{
			/*	change over to the next animation, if their is no next animation 
				(if set to -1) no animation will be selected and 
				getAnimationStartingAndEndingFrame will not select any start and end 
				frames */
			currentAnimInfo.currentAnimationState = currentAnimInfo.nextAnimationState;
			getAnimationStartingAndEndingFrame(currentAnimInfo.currentAnimationState, 
								currentAnimInfo.startFrame, currentAnimInfo.endFrame);
			/*	set frame and add on the amount by which the joining 
				start frame was exceeded */
			currentFrame = currentAnimInfo.startFrame+amtOver;
		}		
	}	
}

void CNebulus::saveReplayUpdate(ReplayData& data, bool)
{
	if (ReplayManager::instance()->IsRecording())
	{
		data.scale = drawScale;
	}
	else // replaying
	{
		drawScale = data.scale;
	}
}

// calculate the final matrix
void CNebulus::calculateFinalMatrix()
{
	finalMatrix.identity(); // set final matrix to identity
	finalMatrix.SetMatrixTranslation(CVector4(position, 1.0f)); // set translation
	finalMatrix *= &rotation; // set rotation

	// create scaling matrix
	static CMatrix theScale; 
	theScale.creatScalingMatrix(drawScale.x, drawScale.y, drawScale.z); 
	finalMatrix *= &theScale; // set scale
}

// get the starting and ending frames for the animation required
void CNebulus::getAnimationStartingAndEndingFrame(int animation, float &startFrame, 
												  float &endFrame)
{
	switch(animation)
	{
		case WALKING:
			startFrame = WALK_START;
			endFrame = WALK_END;
			break;
		case STANDING:
			startFrame = STAND_START;
			endFrame = STAND_END;
			break;
		case STANDING_TO_WALKING: 
			startFrame = STANDING_TO_WALKING_START;
			endFrame = STANDING_TO_WALKING_END;
			break;
		case WALKING_TO_STANDING:
			startFrame = WALKING_TO_STANDING_START;
			endFrame = WALKING_TO_STANDING_END;
			break;
		default: break;
	}
}

CTunnel* CNebulus::GetTunnel()
{
	return mpTunnel;
}

// response to hitting a perticular sprite
void CNebulus::collisionResponse(CSprite * collidedWith, 
								 float perCentOfMoveComplete,  // for evironment collision
								CVector * polyIntersectionPoint, 
								CVector * spriteOldPosition, 
								CVector * veloc) // for environment collision
{
	// perform response such as sliding if the environment has been hit
	switch(collidedWith->dynamicProperty) // STATIC_ENVIRONMENT, MOVING etc
	{
		case STATIC_ENVIRONMENT:
		case MOVING_ENVIRONMENT:
		{	/*	nebulus cannot collide with the buffer step, this is specifically to buffer 
				enemies */
			if (collidedWith->spriteType != SBUFFER_STEP)
			{
#ifdef DETERMINISTIC_REPLAY_DEBUG
				ReplayManager::instance()->VectorResult(*veloc);
				ReplayManager::instance()->VectorResult(*spriteOldPosition);
#endif
				// get the intersecting position
				CVector intersectingPosition = (*spriteOldPosition + veloc);
#ifdef DETERMINISTIC_REPLAY_DEBUG
				ReplayManager::instance()->FloatResult(perCentOfMoveComplete);
#endif
				// set old position
				*spriteOldPosition += (*veloc * perCentOfMoveComplete);
#ifdef DETERMINISTIC_REPLAY_DEBUG
				ReplayManager::instance()->VectorResult(*spriteOldPosition);
#endif

				// get the normal of collision
				CVector collidedWithNormal = *spriteOldPosition - polyIntersectionPoint;			
				collidedWithNormal.normalise();
				/*	get vector from sphere intersection point to poly intersection point */
				CVector	temp = (*polyIntersectionPoint - (intersectingPosition - collidedWithNormal)),
						norm = collidedWithNormal * (temp.dotProduct(&collidedWithNormal));
	
				intersectingPosition += &norm;
				*veloc = intersectingPosition - spriteOldPosition;
				
				// take away velocity in the direction of the normal collided with
				collidedWithNormal *= &(scaledElipsoidRadiusVector); // convert out of elipsoid space
				collidedWithNormal.normalise();
				
				velocity -= ( collidedWithNormal * (velocity.dotProduct(collidedWithNormal)) );
#ifdef DETERMINISTIC_REPLAY_DEBUG
				ReplayManager::instance()->VectorResult(velocity);
#endif
	
				// now in contact with a surface
				currentlyInSurfaceContact = true;

				// see if surface is too steep to allow Nebulus to jump on it
				CVector upVect(0.0f, 1.0f, 0.0f);
				// 0.0f is virtical
				if (upVect.dotProduct(&collidedWithNormal) > 0.3f){
					onJumpableSurface = true;}
			}// end if collidedWith->spriteType != SBUFFER_STEP
			break;
		}
		default: break;
	}// end switch spriteType

	// perform specific responses to hitting different sprites
	switch(collidedWith->spriteType) // collapsing steps, enemeies etc
	{
		case SBOUNCING_SIDE_TO_SIDE_ENEMY:
		case SSIDE_TO_SIDE_ENEMY:
		case SFLY_ACROSS_ENEMY:
		case SUP_DOWN_ENEMY: // Nebulus has hit the up down enemy
		{
			ShootOut();
			break;
		}// end case SUP_DOWN_ENEMY
		case SEXIT_SCREEN: // nebulus has completed the level
			levelCompleted = true; // nebulus has completed the level
			break;
		case STUNNEL:
			mpTunnel = (CTunnel*)collidedWith;
			break;
		default: break;
	}// end switch
}

void CNebulus::ShootOut()
{
	// work out a vector to shoot Nebulus off the tower 
	CVector impactVector = position;
	impactVector.y = 0.0f;
	impactVector.normalise();
	impactVector.setMagnitude((NEBULUS_FORWARD_THRUST*2.1f)*theTower->xPercentFromBaseMeasurement);
	impactVector.y = NEBULUS_JUMP_THRUST*0.5f;
	// set the velocity
	velocity = impactVector;

	wasShotOut = true;

	if (!ReplayManager::instance()->IsReplayingEndLevel())
	{
		soundPlayedThisFrame = SOUNDS_KNOCKEDOFFPLATFORM;
		Globals::Instance().sound.PlaySound( SOUNDS_KNOCKEDOFFPLATFORM, false ); // play knocked off the platform sound once
	}
}

	
