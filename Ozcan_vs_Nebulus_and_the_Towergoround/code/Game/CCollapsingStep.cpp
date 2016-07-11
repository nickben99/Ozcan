//CCollapsingStep.cpp, the collapsing step class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CCollapsingStep.h"
//-------------------------

// constructor
CCollapsingStep::CCollapsingStep(CModel * aModel, int towerPosition)
	: CTowerStep(aModel, towerPosition) // initialise super class
{
	initialise();
}

// initialise the class
void CCollapsingStep::initialise( void)
{
	hasBeenSteppedOn = false;
	timeTillCollapse = 0; // initialise to 0
	mass = 50.0f; // high mass means rapid fall to the ground
	// set sprite type
	spriteType = SCOLLAPSING_STEP;
	// set dynamic property
	dynamicProperty = MOVING_ENVIRONMENT;

	//	work out which axis the collapsing steps model is largest on
	float width;
	if (scaledBoundingBox.x > scaledBoundingBox.y)
	{
		if (scaledBoundingBox.x > scaledBoundingBox.z){
			width = scaledBoundingBox.x;}
		else{
			width = scaledBoundingBox.z;}
	}
	else
	{
		if (scaledBoundingBox.y > scaledBoundingBox.z){
			width = scaledBoundingBox.y;}
		else{
			width = scaledBoundingBox.z;}
	}

	/*	set the radius a sphere would need to be to compleatly 
		encapsulate the collapsing steps model */
	boundingBoxRadius = width/( (2.0f*(float)sqrt(3.0f)) / 3.0f );

	// so sprite will bedrawn in correct position after reinitialising
	oldPosition = position;
	calculateFinalMatrix();
}

// reinitialise the sprite to its starting position
void CCollapsingStep::reinitialise( void)
{
	CTowerStep::reinitialise();
	CCollapsingStep::initialise();
}

// destructor
CCollapsingStep::~CCollapsingStep()
{
	CTowerStep::~CTowerStep(); // call super class destructor
}

// move the collapsing step
void CCollapsingStep::move(int timeChange, GLfloat perCentOfSecond)
{
	forces.reset(); // reset forces (new forces will be added below
	
	/*	if the timer has been started and half a second has elapsed, start moving the step groundward */
	if (hasBeenSteppedOn)
	{
		timeTillCollapse += perCentOfSecond;
	}

	if (timeTillCollapse > COLLAPSE_WAIT_PERIOD)
	{
		applyForce((gravitation * mass));	/*	gravitational force is as F = m * g. 
												(mass times the gravitational acceleration) */

		/* apply air friction force (causes terminal velocity so step will not
									continue to accelerate downwards, velocity will eventually
									level off) */
		applyForce((-velocity * 0.52f)); // airFrictionConstant originally 0.02f
		// call super class move method

		// the step is moving so change the collision type
		//collisionType = BOUNDING_SPHERE;
	}
	
	CSprite::move(timeChange, perCentOfSecond);
}

// collision response
void CCollapsingStep::collisionResponse(	CSprite * collidedWith,
											float perCentOfMoveComplete, 
											CVector * polyintersectionPoint, 
											CVector * spriteOldPosition, 
											CVector * veloc) 
{
	// see what has hit the step
	switch(collidedWith->spriteType)
	{
		case SNEBULUS:
		{
			// if the timer has not already started
			if (!hasBeenSteppedOn)
			{
				// get the sprite position at the time of collision and the poly intersection point
				CVector spritPositionOnCollision = 
							*spriteOldPosition+(*veloc * perCentOfMoveComplete),
						polyintersectPos = *polyintersectionPoint, upVector(0.0f, 1.0f, 0.0f);

				// convert out of ellipsoid space
				spritPositionOnCollision *= &collidedWith->scaledElipsoidRadiusVector;
				polyintersectPos *= &collidedWith->scaledElipsoidRadiusVector;

				// work out the angle of collision
				CVector angleOfImpact = spritPositionOnCollision - &polyintersectPos;
				angleOfImpact.normalise();

				/*	see if the angle of impact is roughly equal to a hit on the top of the 
					collapsing step */
				bool landedOnTopOfStep =	(fabs(angleOfImpact.x - (upVector.x)) < 0.1f) && 
											(fabs(angleOfImpact.y - (upVector.y)) < 0.1f) &&
											(fabs(angleOfImpact.z - (upVector.z)) < 0.1f);

				/*	start the timer if nebulus has landed on the top
					of the step */
				if (landedOnTopOfStep)
				{
					hasBeenSteppedOn = true;
				}
			}
			break;
		}
	}
}