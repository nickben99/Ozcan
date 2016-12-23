//CLift.cpp, the tower lift class

// system includes --------
#include <math.h>
#include <iostream>
//-------------------------

// header files -----------
#include "CLift.h"

//-------------------------

// constructor
CLift::CLift(CModel * aModel, int towerPosition, char LiftTopIdentifier)
	: CTowerStep(aModel, towerPosition) // initialise super class
{
	liftTopYHeight = -1;
    
//    for (int i = 0; i < CTower::instance()->totalTowerObjects; i++){
//        char val = CTower::instance()->GetTowerObject(i);
//        std::cout << "\n " + std::to_string(i) + ". " + std::to_string(val);
//    }
    
	// find the top of the lift
	for (int object = towerPosition+theTower->towerColumns; 
									object < theTower->totalTowerObjects; 
												object+=theTower->towerColumns)
	{
		// set the height of the top of the lift	
		if (LiftTopIdentifier == theTower->GetTowerObject(object))
		{
			int startingRow		= object/theTower->towerColumns;
			liftTopYHeight = (startingRow*theTower->towerRowHeight);
			// find difference in height between the lift top and bottom
			distanceToLiftTop = (int)(liftTopYHeight-position.y);
			break; // lift top found so break out of for loop
		}
	}

	// set the collision type for moving object collisions
	collisionType = BOUNDING_CAPSULE;
	// set position and radius
	position += CVector(0.0f, distanceToLiftTop*0.5f, 0.0f);
	boundingSphereRadius = 
		0.7f*(theTower->towerWallPos+(scale.z*(BASE_MEASUREMENT_Z*0.5f)))
			*tan(degToRad(theTower->angleBetweenTowerSegments*0.5f));
	
	// set height of capsule
	height = distanceToLiftTop - boundingSphereRadius;
	// considered to be a moving sprite
	dynamicProperty = MOVING;
	// set sprite type
	spriteType = SLIFT_BOTTOM;

	boundingBox = 
		CVector(boundingSphereRadius*2.0f, (float)distanceToLiftTop, boundingSphereRadius*2.0f);
	scaledBoundingBox = boundingBox; // scaling already done above 
	scaledHalfBoundingBox = scaledBoundingBox*0.5f;
	scaledElipsoidRadiusVector = CVector(); // not used for lifts
}

// destructor
CLift::~CLift()
{
	CTowerStep::~CTowerStep(); // call super class destructor
}

// collision response
void CLift::collisionResponse(	CSprite * collidedWith,
								float, 
								CVector*, 
								CVector*, 
								CVector*) 
{
	switch(collidedWith->spriteType)
	{
		case SNEBULUS:
		{
			// set Nebulus's velocity and shoot him into the air to the lift top position
			collidedWith->gravitation = CVector(0.0f, 9.81f, 0.0f);
			break;
		}
        default: break;
	}
}