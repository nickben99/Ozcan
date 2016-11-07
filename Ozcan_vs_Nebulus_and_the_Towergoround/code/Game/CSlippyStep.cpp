//CSlippyStep.cpp, the slippy step class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CSlippyStep.h"

//-------------------------

// constructor
CSlippyStep::CSlippyStep(CModel * aModel, int towerPosition, bool slippyStepLeft)
	: CTowerStep(aModel, towerPosition) // initialise super class
{
	// set sprite type
	spriteType = SSLIPPY_STEP;
	// set collision type
	//collisionType = ORIENTED_BOUNDING_RECTANGLE;
	
	/*	if this is a left slippy step create a force facing left, 
		if not create the force facing right */
	if (slippyStepLeft){
		forceVector.x = -1.0f;}
	else{
		forceVector.x = 1.0f;}

	// rotate by the steps rotation
	forceVector = rotation.multiplyRotateVector(&forceVector);
}

// destructor
CSlippyStep::~CSlippyStep()
{
	CTowerStep::~CTowerStep(); // call super class destructor
}

// collision response
void CSlippyStep::collisionResponse(CSprite * collidedWith,
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
			// work out the angle of impact
			CVector angleOfImpact = *spriteOldPosition+(*veloc * perCentOfMoveComplete)
																- polyintersectionPoint, 
					upVector(0.0f, 1.0f, 0.0f);

			angleOfImpact *= &collidedWith->elipsoidRadiusVector;
			angleOfImpact.normalise();

			/*	apply force if nebulus has landed on the top
				of the step */
			if (angleOfImpact.fuzzyEquals(&upVector)){
				/*	apply the force to nebulus, so his velocity vector is larger than
					the force pushing him back */
				collidedWith->applyForce(forceVector*5.0f);}
			break;
		}
        default: break;
	}
}