//CFireBall.cpp, a fire ball class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CFireBall.h"
#include "ReplayManager.h"

#include "Game/Globals.h"
//-------------------------

// constructor
CFireBall::CFireBall()
{
}

// overloaded constructor
CFireBall::CFireBall(CModel * aModel)
	: CSprite(aModel) 
{
	initialise();
}

// initilise class
void CFireBall::initialise()
{
	collisionType = BOUNDING_ELIPSOID; /*	the fire ball is represented by a bounding 
											ellipsoid for collision detection */
	spriteType = SFIRE_BALL; // set sprite type
	dynamicProperty = MOVING; // the fire ball is a moving sprite
	scale = CVector(1.0f, 1.0f, 1.0f); // unscaled
	
	// set fire ball's radius vector
	elipsoidRadiusVector = CVector(boundingSphereRadius, boundingSphereRadius, 
									boundingSphereRadius); 

	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

	forwardThrust = 7.0f; // starting forward acceleration per second 
	upThrust = 4.0f; // starting up acceleration per second
	surfaceFrictionConstant = 1.52f;
	airFrictionConstant = 0.0f; // 0.52f
	mass = 1.0f; // set fire ball mass

	active = false; // initially inactive, untill thrown
	theNebulus = CNebulus::getExistingInstance(); // get Nebulus instance
	currentlyInSurfaceContact = false; // starting not in contact with a surface
	fireBallTimer = 0.0f; 

	// so sprite will bedrawn in correct position after reinitialising
	oldPosition = position = CVector::zero;
	calculateFinalMatrix(); 
}

// reinitialise sprite to starting state
void CFireBall::reinitialise( void)
{
	CSprite::reinitialise(); // call CSprite reinitialise first (same order as when constructed)
	CFireBall::initialise(); // call this class' initialise method
}

// destructor
CFireBall::~CFireBall()
{
	CSprite::~CSprite();
}

// deal with input control to the fire ball
void CFireBall::keyInput()
{
	soundPlayedThisFrame = SOUNDS_COUNT;
	// throw the fire ball
	if(Globals::Instance().keys.GetKeyPressed(eKeyCode_N, true) && (!active))
	{
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//		ReplayManager::instance()->IntResult(1111);
//#endif
		position = theNebulus->position;
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//		ReplayManager::instance()->VectorResult(position);
//#endif

		// set velocity based on how fast nebulus is going
		velocity = CVector(0.0f, upThrust*0.2f, forwardThrust); 
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//		ReplayManager::instance()->VectorResult(velocity);
//#endif
		/*	rotate velocity by Nebulus rotation so the fire ball will shoot out 
			in the direction Nebulus is facing */
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//		ReplayManager::instance()->MatrixResult(theNebulus->rotation);
//#endif
		velocity = theNebulus->rotation.multiplyRotateVector(&velocity);
#ifdef DETERMINISTIC_REPLAY_DEBUG
		ReplayManager::instance()->VectorResult(velocity);
#endif
		active = true;
		fireBallTimer = 0.0f; // start timer
		scale = theNebulus->scale; // reset the scale

		// set current bounding box and elipsoid radius vectors based on scale
		scaledBoundingBox = boundingBox * &scale;
		scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
		scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

		if (!ReplayManager::instance()->IsReplayingEndLevel())
		{
			soundPlayedThisFrame = SOUNDS_THROWFIREBALL;
			Globals::Instance().sound.PlaySound( SOUNDS_THROWFIREBALL, false ); // play throw fireball sound once
		}
	}
}

// move the sprite
void CFireBall::move(int timeChange, GLfloat perCentOfSecond)
{
	// move fire ball if its active
	if (active)
	{
		fireBallTimer += perCentOfSecond; // add on perCentOfSecond to timer
#ifdef DETERMINISTIC_REPLAY_DEBUG
		ReplayManager::instance()->FloatResult(fireBallTimer);
#endif
		applyForce((gravitation * mass));	//gravitational force is as F = m * g. (mass times the gravitational acceleration)
	
		/*	apply air friction force (causes terminal velocity so fire ball will not
			continue to accelerate downwards, velocity will eventually level off) */
		applyForce((-velocity * airFrictionConstant)); // airFrictionConstant
	
		CSprite::move(timeChange, perCentOfSecond); // integrate movement
	
		currentlyInSurfaceContact = false; /*	reset to false (may become true 
												during collision detection and response) */
		forces.reset(); /*	reset forces (new forces will be added during collision response 
							and this move method in the next cycle) */

		// if fire ball has run out
		if (fireBallTimer > FIRE_BALL_TIME_PERIOD)
		{
			// get percentage of shrink
			float perCentOfShrink = 
				(fireBallTimer-FIRE_BALL_TIME_PERIOD)/DISAPPEAR_TIME_PERIOD;
			
			// if perCentOfShrink is greater than 1.0f
			if (perCentOfShrink > 1.0f){
				active = false; // no longer active
				perCentOfShrink = 1.0f;} // cap shrink at 1.0f
	
			// shrink fire ball
			scale = 
				CVector(1.0f-perCentOfShrink, 1.0f-perCentOfShrink, 1.0f-perCentOfShrink);

			// set current bounding box and elipsoid radius vectors based on scale
			scaledBoundingBox = boundingBox * &scale;
			scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
			scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;
		} 
	}
}

// draw the sprite
void CFireBall::draw(int timeChange)
{
	if (active){ // draw the sprite if its active
		CSprite::draw(timeChange);}
}

// response to hitting a perticular sprite
void CFireBall::collisionResponse(	CSprite * collidedWith, 
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

				// get the intersecting position
				CVector intersectingPosition = (*spriteOldPosition + veloc);
				// set old position
				*spriteOldPosition += (*veloc * perCentOfMoveComplete);
				// get the normal of collision
				CVector collidedWithNormal = *spriteOldPosition - polyIntersectionPoint;			
				collidedWithNormal.normalise();
				/*	get vector from sphere intersection point to poly intersection point */
				CVector	temp = (*polyIntersectionPoint - (intersectingPosition - collidedWithNormal)),
						norm = collidedWithNormal * (temp.dotProduct(collidedWithNormal));

				intersectingPosition += (norm*2.0f);
				*veloc = intersectingPosition - spriteOldPosition;
			
				// reflect fireball off the surface collided with
				collidedWithNormal *= (elipsoidRadiusVector * scale); // convert out of elipsoid space
				collidedWithNormal.normalise();

				CVector projection = 
					collidedWithNormal * ( -velocity.dotProduct(&collidedWithNormal) );

				velocity += ( projection*2.0f ); // reflect off the surface
			}
			break;
		}
		default: break;
	}// end switch spriteType

	// perform specific responses to hitting different sprites
	switch(collidedWith->spriteType) // side to side enemies etc
	{
		case SBOUNCING_SIDE_TO_SIDE_ENEMY:
		case SFLY_ACROSS_ENEMY:
		case SSIDE_TO_SIDE_ENEMY: // side to side enemy struck
			active = false; // an enemy has been destroyed so inactivate fire ball
			theNebulus->score += collidedWith->killValue; // increase nemulus' score
			break;
		default: break;
	}// end switch
}



