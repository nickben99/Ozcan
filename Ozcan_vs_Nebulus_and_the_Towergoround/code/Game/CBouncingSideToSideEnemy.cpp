//CBouncingSideToSideEnemy.cpp, the bouncing side to side enemy class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CBouncingSideToSideEnemy.h"
#include "ReplayManager.h"
#include "Game/Globals.h"

//-------------------------

// constructor
CBouncingSideToSideEnemy::CBouncingSideToSideEnemy(CModel * aModel, int towerPosition)
	: CTowerStep(aModel, towerPosition) // initialise super class
{
	initialise();
}

// initialise the class
void CBouncingSideToSideEnemy::initialise( void)
{
	particles.initialise();
	collisionType = BOUNDING_ELIPSOID; /*	the up down enemy is represented by a 
											bounding ellipsoid for collision detection */
	mass = 1.0f; // high mass means rapid fall to the ground
	// set sprite type
	spriteType = SBOUNCING_SIDE_TO_SIDE_ENEMY;
	// set dynamic property
	dynamicProperty = MOVING;
	// set y scale to z scale so model does not appear stretched
	scale.y = scale.x;

	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

	CMatrix startingRotation;
	// start the side to side enemy rolling in a random direction
	startingRotation.createYRotationMatrix(degToRad(FRANDOM(0, 359)));
	velocity = startingRotation.multiplyRotateVector(CVector(0.5f*scale.x, 0.0f, 0.0f));

	killValue = 300; // point value worth when killed by nebulus' fireball

	// so sprite will bedrawn in correct position after reinitialising
	oldPosition = position;
	calculateFinalMatrix();
}

// reinitialise the sprite to its starting position
void CBouncingSideToSideEnemy::reinitialise( void)
{
	CTowerStep::reinitialise();
	CBouncingSideToSideEnemy::initialise();
}

// destructor
CBouncingSideToSideEnemy::~CBouncingSideToSideEnemy()
{
}

// move the sprite
void CBouncingSideToSideEnemy::move(int timeChange, GLfloat perCentOfSecond)
{
	soundPlayedThisFrame = SOUNDS_COUNT;
	// move the sprite if its active
	if (active)
	{
		forces.reset();// reset forces before adding them below for this frame

		applyForce((gravitation * mass));	/*	gravitational force is as F = m * g. 
												(mass times the gravitational acceleration) */ 
	
		CSprite::move(timeChange, perCentOfSecond); // integrate movement

		// work out amount to rotate side to side enemy based on how far its travelled
		float	sphereCircumferance = 2.0f*PI*(boundingSphereRadius*scale.x),
				distTravelled		= (position - &oldPosition).getMagnitude(),
				amtToTurn			= (distTravelled / sphereCircumferance) * (2.0f*PI);

		// get turning axis
		CVector turnAxis = velocity;
		turnAxis.y = 0.0f;
		turnAxis.x = velocity.z;
		turnAxis.z = -velocity.x;
		turnAxis.normalise();

		// create turning matrix
		CMatrix turn;
		turn.createArbitraryAxisRotation(amtToTurn, turnAxis);
		/*	add matrix to sprites rotation matrix (must be turn * rotation, 
			rotation * turn would not rotate correctly */
		rotation = turn * &rotation;
	}	
	particles.move(perCentOfSecond);
}

void CBouncingSideToSideEnemy::saveReplayUpdate(ReplayData& data, bool firstFrame)
{
	particles.saveReplayUpdate(data.particleSystemActive, data.particleSystemFrame, firstFrame, &position);
}

// draw the sprite
void CBouncingSideToSideEnemy::draw(int timeChange)
{
	if (active){
		CSprite::draw(timeChange);}

	particles.draw();
}

// collision response
void CBouncingSideToSideEnemy::collisionResponse(	CSprite * collidedWith, 
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

			// bounce off the surface collided with
			collidedWithNormal *= (elipsoidRadiusVector * scale); // convert out of elipsoid space
			collidedWithNormal.normalise();

			CVector projection = // get projection of velocity onto normal of collision
				collidedWithNormal * ( -velocity.dotProduct(&collidedWithNormal) );

			intersectingPosition += (norm*2.0f); // set position following hit
			*veloc = intersectingPosition - spriteOldPosition;

			velocity += (projection * 2.0f); // reflect off the surface
			break;
		}// end case STATIC_ENVIRONMENT
		default: break;
	}// end switch spriteType

	// perform specific responses to hitting different sprites
	switch(collidedWith->spriteType) // other side to side enemies etc
	{
		case SBOUNCING_SIDE_TO_SIDE_ENEMY: // another bouncing side to side enemy struck
		{	// work out relative velocity and collision normal
			CVector relativeVelocity	=	velocity - &collidedWith->velocity, 
					collisionNormal		=	(position - &collidedWith->position) / 
											(position - &collidedWith->position).getMagnitude();

			/*	if relative velocity is less than zero the side to side enemies are headed for one and
				other, equal to zero means they have the same velocity and will remain in contact, 
				greater than zero means they are headed away from each other.  After the impulse 
				force is applied the sprites will be headed away from each other, meaning the following 
				section of code will only be executed for one of the enemies after a collision */
			if ((relativeVelocity.dotProduct(&collisionNormal)) < 0.0f)
			{	// compute impulse
				float impulse	=	
					(-(1.0f+COEFFICIENT_OF_RESTITUTION) * (relativeVelocity.dotProduct(&collisionNormal))) / 
					((collisionNormal.getMagnitudeSquared()) * 
					((1.0f/mass) + (1.0f/collidedWith->mass)));

				/*	apply impulse to this sprite and impulse negated to the sprite 
					collided with */
				velocity += ((collisionNormal * impulse) / mass);
				collidedWith->velocity += ((collisionNormal * -impulse) / mass);
			}

			break;
		}// end case SBOUNCING_SIDE_TO_SIDE_ENEMY
		case SFIRE_BALL: // nebulus's fire ball has been struck

			if (!ReplayManager::instance()->IsReplayingEndLevel())
			{
				active = false; // no longer active (enemy has been killed)
				// start the particle explosion
				particles.resetAllParticles(&position);
				transparency = true; // so particle system is drawn last (essential)

				soundPlayedThisFrame = SOUNDS_ENEMYEXPLODE;
				Globals::Instance().sound.PlaySound( SOUNDS_ENEMYEXPLODE, false ); // play enemy explode sound once
			}
			break;
		default: break;
	}// end switch
}