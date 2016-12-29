//CUpDownEnemy.cpp, the up-down enemy class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CUpDownEnemy.h"

//-------------------------

// constructor
CUpDownEnemy::CUpDownEnemy(CModel * aModel, int towerPosition)
	: CTowerStep(aModel, towerPosition) // initialise super class
{
	initialise();
}

// initialise class
void CUpDownEnemy::initialise( void)
{
	collisionType = BOUNDING_ELIPSOID; /*	the up down enemy is represented by a 
											bounding ellipsoid for collision detection */
	mass = 50.0f; // high mass means rapid fall to the ground
	// set sprite type
	spriteType = SUP_DOWN_ENEMY;
	// set dynamic property
	dynamicProperty = MOVING;
	// set y scale to z scale so model does not appear stretched
	scale.y = scale.x;
	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

	// so sprite will bedrawn in correct position after reinitialising
	oldPosition = position;
	calculateFinalMatrix();
}

// set sprite back to starting state
void CUpDownEnemy::reinitialise( void)
{
	CTowerStep::reinitialise();
	CUpDownEnemy::initialise(); // call this class' constructor
}

// destructor
CUpDownEnemy::~CUpDownEnemy()
{
}

// move the sprite
void CUpDownEnemy::move(int timeChange, GLfloat perCentOfSecond)
{
	forces.reset();// reset forces before adding them below for this frame

	applyForce((gravitation * mass));	/*	gravitational force is as F = m * g. 
											(mass times the gravitational acceleration) */

	/*	apply air friction force causes terminal velocity so 
		velocity will eventually level off) */
	applyForce(-velocity * airFrictionConstant); 

	// create a matrix to rotate the sprite the required amount this frame
	CMatrix rotationThisFrame;
	rotationThisFrame.createYRotationMatrix(UP_DOWN_ENEMY_ROTATE_SPEED*perCentOfSecond);

	rotation *= &rotationThisFrame; // add the new rotation onto the sprites rotation

	CSprite::move(timeChange, perCentOfSecond); // integrate movement
}

// collision response
void CUpDownEnemy::collisionResponse(	CSprite * collidedWith, 
										float perCentOfMoveComplete,  // for evironment collision
										CVector * polyIntersectionPoint, 
										CVector * spriteOldPosition, 
										CVector * veloc) // for environment collision
{
	// perform response such as sliding if the environment has been hit
	switch(collidedWith->dynamicProperty) // STATIC_ENVIRONMENT, MOVING etc
	{
		case STATIC_ENVIRONMENT:
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
					norm = collidedWithNormal * (temp.dotProduct(&collidedWithNormal));

			intersectingPosition += &norm;
			*veloc = intersectingPosition - spriteOldPosition;
			
			// take away velocity in the direction of the normal collided with
			collidedWithNormal *= (elipsoidRadiusVector * scale); // convert out of elipsoid space
			collidedWithNormal.normalise();
			
			velocity -= ( collidedWithNormal * (velocity.dotProduct(collidedWithNormal)) );

			/*	after hitting surface, up-down enemies gravitational acceleration is 
				inverted */
			gravitation.y = -gravitation.y;
			break;
		}
		default: break;
	}// end switch spriteType
}