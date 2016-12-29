//CSideToSideEnemy.cpp, the side to side enemy class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CSideToSideEnemy.h"
#include "ReplayManager.h"
#include "Game/Globals.h"
//-------------------------

// constructor
CSideToSideEnemy::CSideToSideEnemy(CModel * aModel, int towerPosition)
	: CTowerStep(aModel, towerPosition) // initialise super class
{
	initialise();
}

// initialise class
void CSideToSideEnemy::initialise()
{
	particles.initialise();
	collisionType = BOUNDING_ELIPSOID; /*	the up down enemy is represented by a 
											bounding ellipsoid for collision detection */
	mass = 1.0f; // high mass means rapid fall to the ground
	// set sprite type
	spriteType = SSIDE_TO_SIDE_ENEMY;
	// set dynamic property
	dynamicProperty = MOVING;
	// set y scale to z scale so model does not appear stretched
	scale.y = scale.x;
	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

	airFrictionConstant = 3.0f; /*	set air friction constant (only applied on y axis 
									so their is no loss of ground plane velocity) */
	CMatrix startingRotation;
	// start the side to side enemy rolling in a random direction
	startingRotation.createYRotationMatrix(degToRad(FRANDOM(0, 359)));
	velocity = 
		startingRotation.multiplyRotateVector(CVector(3.0f*scale.x, 0.0f, 0.0f));

	killValue = 100; // point value worth when killed by nebulus' fireball

	// so sprite will bedrawn in correct position after reinitialising
	oldPosition = position;
	calculateFinalMatrix();
}

// reinitialise to starting state
void CSideToSideEnemy::reinitialise( void)
{
	CTowerStep::reinitialise();
	CSideToSideEnemy::initialise(); // call this class' constructor
}

// destructor
CSideToSideEnemy::~CSideToSideEnemy()
{
}

// move the sprite
void CSideToSideEnemy::move(int timeChange, GLfloat perCentOfSecond)
{
	soundPlayedThisFrame = SOUNDS_COUNT;
	if (active)
	{
		forces.reset();// reset forces before adding them below for this frame

		applyForce((gravitation * mass));	/*	gravitational force is as F = m * g. 
												(mass times the gravitational acceleration) */

		/*	apply air friction force only on the y axis this means the side to side 
			enemy will not lose energy on the ground plane */
		CVector upVector(0.0f, 1.0f, 0.0f),
					virticalVeloc = (upVector * (velocity.dotProduct(upVector)));
		applyForce((-virticalVeloc * airFrictionConstant)); 
	
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
	
	if (particles.systemActive){ // if any particles are still alive
		particles.move(perCentOfSecond);}
}

void CSideToSideEnemy::saveReplayUpdate(ReplayData& data, bool firstFrame)
{
	particles.saveReplayUpdate(data.particleSystemActive, data.particleSystemFrame, firstFrame, &position);
}

// draw the sprite
void CSideToSideEnemy::draw(int timeChange)
{
	if (active){ // if the sprite is still active
		CSprite::draw(timeChange);}

	if (particles.systemActive){ // if any particles are still alive
		particles.draw();}
}

// collision response
void CSideToSideEnemy::collisionResponse(	CSprite * collidedWith, 
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

			// bounce or slide along surface collided with
			collidedWithNormal *= (elipsoidRadiusVector * scale); // convert out of elipsoid space
			collidedWithNormal.normalise();

			CVector projection = // get projection of velocity onto normal of collision
				collidedWithNormal * ( -velocity.dotProduct(&collidedWithNormal) );
			
			/*	if the velocity in the direction of the surface collided with is very 
				small, roll along the surface rather than bouncing off it, in this case to preserve 
				a constant ground plane velocity, only the ground plane can be slided upon */
			if (projection.getMagnitude() < 1.0f &&	
				(collidedWithNormal / collidedWithNormal.getMagnitude()).fuzzyEquals(CVector(0.0f, 1.0f, 0.0f))) 
			{
				intersectingPosition += &norm;
				*veloc = intersectingPosition - spriteOldPosition;
			
				velocity -= ( collidedWithNormal * 
					(velocity.dotProduct(collidedWithNormal)) );
			}
			else // bounce off the surface
			{
				intersectingPosition += (norm*2.0f);
				*veloc = intersectingPosition - spriteOldPosition;

				velocity += (projection * 2.0f); // reflect off the surface
			}
			break;
		}// end case STATIC_ENVIRONMENT
		default: break;
	}// end switch spriteType

	// perform specific responses to hitting different sprites
	switch(collidedWith->spriteType) // other side to side enemies etc
	{
		case SSIDE_TO_SIDE_ENEMY: // another side to side enemy struck
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
		}// end case SSIDE_TO_SIDE_ENEMY
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