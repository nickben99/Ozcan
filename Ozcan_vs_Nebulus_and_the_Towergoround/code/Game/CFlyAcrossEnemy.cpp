//CFlyAcrossEnemy.cpp, the tower bottom class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CFlyAcrossEnemy.h"
#include "ReplayManager.h"
#include "Game/defines.h"
#include "Game/Globals.h"

//-------------------------

// constructor
CFlyAcrossEnemy::CFlyAcrossEnemy(CModel * aModel)
	: CTowerStep(aModel) // initialise super class
{
	initialise();
}

// initialise the class
void CFlyAcrossEnemy::initialise()
{
	CTowerSprite::initialise(0);
	CTowerStep::initialise(0); /*	use the tower sprite and step initialise methods 
									to ensure the fly across enemy has the correct 
									proportions */	
	particles.initialise();
	active = false; // starts off inactive
	regenerationTimer = 0.0f; // start the timer
	anglesTravelledThrough = 0.0f; // set to zero
	resizingRequired = GROW; // will need to go when first generated
	// set sprite type
	spriteType = SFLY_ACROSS_ENEMY;
	// set collision type
	collisionType = BOUNDING_SPHERE;
	// set y scale to z scale so model does not appear stretched
	scale.y = scale.x;

	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

	normalScale = scale.x; /*	record the normal size scale of the fly accross enemy
								the x, y, z scale of the sprite will all be equal */

	// get pointer to Nebulus
	theNebulus = CNebulus::getExistingInstance();

	dynamicProperty = MOVING; // set dynamic property

	CVector positionXZ = position;
	positionXZ.y = 0.0f; /* enemy will stay the same distence from the tower 
							on the groundplane */
	/*	angular velocity is a full circle divided by the total distace around 
		the tower in meters divided by the speed in meters travelling at 
		for example if circumferance is 8 meters and the speed is 4 meters
		360/(8/4) = 180 per second, so two seconds to circle the tower.  If the 
		tower is 16 meters 360/(16/4) = 90 per second, so 4 seconds to circle the tower. 
		so if the length to travel is doubled the time taken is doubled (which is the results
		required) */
	towerCircumferance = 2.0f*PI*positionXZ.getMagnitude(); // get circumferance
	angularVelocity = (2.0f*PI)/(towerCircumferance/7.0f); // in seconds

	killValue = 200; // point value worth when killed by nebulus' fireball

	// so sprite will bedrawn in correct position after reinitialising
	oldPosition = position;
	calculateFinalMatrix();
}

// reinitialise the sprite to its starting position
void CFlyAcrossEnemy::reinitialise( void)
{
	CTowerStep::reinitialise(); // call in same order as constructor
	CFlyAcrossEnemy::initialise(); // call this class' initialise (includes CTowerSprite and CTowerStep initialise calls)
}

// destructor
CFlyAcrossEnemy::~CFlyAcrossEnemy()
{
	CTowerStep::~CTowerStep(); // call super class destructor
}

// shrink or grow the sprite (whichever is required)
void CFlyAcrossEnemy::shrinkOrGrow(int shrinkOrGrow)
{
	// get percentage of shrink or grow
	float perCentageOfShrinkGrow = (shrinkGrowTimer / SHRINK_GROW_PERIOD);

	// cap shrink or grow at 1.0f
	if (perCentageOfShrinkGrow > 1.0f){ 
		perCentageOfShrinkGrow = 1.0f;}
		
	// get the percentage of the normalScale
	float currentScale = perCentageOfShrinkGrow*normalScale;

	// switch on the action required
	switch(shrinkOrGrow)
	{
		case SHRINK:
			// shrink sprite down to zero
			scale = CVector(normalScale-currentScale, normalScale-currentScale, 
							normalScale-currentScale);
			break;
		case GROW:
			// grow the sprite to normal scale size
			scale = CVector(currentScale, currentScale, currentScale);
			break;
		default: break;
	}

	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;
}

// move the sprite
void CFlyAcrossEnemy::move(int timeChange, GLfloat perCentOfSecond)
{
	soundPlayedThisFrame = SOUNDS_COUNT;
	if (active) // if the sprite is active
	{
		oldPosition = position;
		// rotate the sprite around the tower
		CMatrix turn;
		turn.createYRotationMatrix(angularVelocity*perCentOfSecond);
		position = turn.multiplyRotateVector(&position);

		// record how far around the tower the sprite has travelled
		anglesTravelledThrough += fabs(angularVelocity*perCentOfSecond);
		
		/*	if a complete revelution of the tower has been completed and 
			currently in grow mode (although the sprite will be compleatly 
			fully grown) */
		if( (anglesTravelledThrough >= (2.0f*PI)) && (GROW == resizingRequired))
		{
			shrinkGrowTimer = 0.0f; // reset timer
			resizingRequired = SHRINK; // go to shrink mode
		}

		shrinkGrowTimer += perCentOfSecond; // add on to timer
		shrinkOrGrow((int)resizingRequired); // resize the sprite

		/*	work out the rotation of the sprite so it always faces directly away 
			from the tower */
		CVector positionXZ = position; // get the ground plane position
		positionXZ.y = 0.0f;
		float	angle = positionXZ.getAngleBetween(CVector(0.0f, 0.0f, 1.0f));

		// get cross product of the two vectors and normalise
		CVector aCross;
		aCross.crossProduct(positionXZ, CVector(0.0f, 0.0f, 1.0f));
		aCross.normalise();

		if (aCross.fuzzyEquals(CVector(0.0f, -1.0f, 0.0f)))
		{
			angle = (2.0f*PI)-angle;
		} // set axis

		// create rotation matrix from angle and axis
		rotation.createYRotationMatrix(-angle);

		calculateFinalMatrix();

		/*	if a complete revelution of the tower has been completed and the sprite has been
			shrunk */
		if( (anglesTravelledThrough >= (2.0f*PI)) & (0.0f == scale.x))
		{
			active = false; // no longer active
			anglesTravelledThrough = 0.0f; // set back to zero in preparation for next generation
			regenerationTimer = 0.0f; // resest regeneration timer
			shrinkGrowTimer = 0.0f;
		}

		if (model) 
		{
			// advance the animation
			model->advanceFrame(timeChange, currentFrame, framesPerSecond, loopingAnimation);
		}
	}
	else // not active
	{
		regenerationTimer += perCentOfSecond; // add on time		
	}

	// move the particles if any are still alive
	if (particles.systemActive){ particles.move(perCentOfSecond);}

	// if the sprite is innactive and the period between generations has run out
	if ((!active) && (regenerationTimer >= PERIOD_BETWEEN_GENERATIONS))
	{
		active = true; // activate
		shrinkGrowTimer = 0.0f; // reset shrink/grow timer
		resizingRequired = GROW; // sprite will need to grow when first generated
		scale = CVector(0.0f, 0.0f, 0.0f); // start with scale of 0 before grow

		// get the fly accross enemies position and nebulus' position (both on ground plane)
		CVector positionXZ = position, 
				nebulusPositionXZ = theNebulus->position;
		positionXZ.y = nebulusPositionXZ.y = 0.0f;

		// randomly select rotating forward or back
		if (RANDOM(0, 1)) // random number between 0 and 1 generated
		{
			angularVelocity = -fabs(angularVelocity); // rotate back
			// left // get the angle 90 degrees in front of nebulus
			float temp = nebulusPositionXZ.x;
			nebulusPositionXZ.x = nebulusPositionXZ.z;
			nebulusPositionXZ.z = -temp;
		}
		else
		{
			angularVelocity = fabs(angularVelocity); // rotate forward
			// right // get the angle 90 degrees behind nebulus
			float temp = nebulusPositionXZ.z;
			nebulusPositionXZ.z = nebulusPositionXZ.x;
			nebulusPositionXZ.x = -temp;
		}

		/*	if nebulus is inside a tunnel and is too close to x,z 0,0 the angle seems to 
			be set at 1.57f (90 degrees, which is ok) */

		// get angle between current position and 90 degrees in front of nebulus
		float	angle = positionXZ.getAngleBetween(&nebulusPositionXZ);

		// get cross product of the two vectors and normalise
		CVector aCross;
		aCross.crossProduct(&positionXZ, &nebulusPositionXZ);
		aCross.normalise();

		/*	if the cross product could not be derived, which could happen if the vectors are 
			at an angle of 180 degrees from one and other and thus parallel and no plane can 
			be derived (also potentially if vectors are in the same direction) */
		if (aCross.fuzzyEquals(CVector())){
			aCross = CVector(0.0f, 1.0f, 0.0f);} // set axis

		// create the rotation matrix
		CMatrix rotateToMatch;
		rotateToMatch.createArbitraryAxisRotation(angle, aCross);
		// rotate the sprites position
		position = rotateToMatch.multiplyRotateVector(&position);
		// set the y position to nebulus' y position
		position.y = theNebulus->position.y;

		move(0, 0.0f); // call this method again to set the sprite y rotation
	}
}

void CFlyAcrossEnemy::saveReplayUpdate(ReplayData& data, bool firstFrame)
{
	particles.saveReplayUpdate(data.particleSystemActive, data.particleSystemFrame, firstFrame, &position);
}

// draw the sprite
void CFlyAcrossEnemy::draw(int timeChange)
{
	if (active){ // draw the sprite if its still alive
		CSprite::draw(timeChange);}

	if (particles.systemActive){ // if any particles are still alive
		particles.draw();}
}

// collision response
void CFlyAcrossEnemy::collisionResponse(	CSprite * collidedWith, 
											float,  // for evironment collision
											CVector*, 
											CVector*, 
											CVector*) // for environment collision
{
	// perform specific responses to hitting different sprites
	switch(collidedWith->spriteType) // other enemies etc
	{
		case SFIRE_BALL: // nebulus's fire ball has been struck

			if (!ReplayManager::instance()->IsReplayingEndLevel())
			{
				active = false; // no longer active (enemy has been killed)
				anglesTravelledThrough = 0.0f; // set back to zero in preparation for next generation
				regenerationTimer = 0.0f; // resest regeneration timer
				shrinkGrowTimer = 0.0f;
				// start the particle explosion
				particles.resetAllParticles(&position);
				soundPlayedThisFrame = SOUNDS_ENEMYEXPLODE;
				Globals::Instance().sound.PlaySound( SOUNDS_ENEMYEXPLODE, false ); // play enemy explode sound once
			}

			break;
		default: break;
	}// end switch
}