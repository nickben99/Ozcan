//CPowerUpNoFriction.cpp, the no friction power up class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CPowerUpNoFriction.h"
#include "ReplayManager.h"
#include "Game/defines.h"

//-------------------------

// constructor
CPowerUpNoFriction::CPowerUpNoFriction(CModel * aModel, char towerSegmentIdentifier)
	: CPowerUp(aModel, towerSegmentIdentifier)
{
	nebulusOriginalFrictionValue = 0.0f; // initialise value
}

// destructor
CPowerUpNoFriction::~CPowerUpNoFriction()
{
	CPowerUp::~CPowerUp();
}

// perform the power-ups action
void CPowerUpNoFriction::move(int timeChange, GLfloat perCentOfSecond)
{	
	// call super class move to rotate the sprite
	CPowerUp::move(timeChange, perCentOfSecond);

	// if power-up has run out
	if (actingUpon != 0) 
	{
		powerUpTimer += perCentOfSecond; // add on perCentOfSecond to timer

		theHUD->powerUpPerCentRemaining = // set hud with time remaining
			1.0f-(powerUpTimer/POWER_UP_TIME_PERIOD);

		if (powerUpTimer > POWER_UP_TIME_PERIOD)
		{
			// set nebulus surface friction back to normal
			actingUpon->surfaceFrictionConstant = nebulusOriginalFrictionValue;
			// power-up is finished so reinitialise in a random location
			CPowerUp::reinitialise();
		}
	}
}

// draw this sprite
void CPowerUpNoFriction::draw(int timeChange)
{
	CPowerUp::draw(timeChange);
} 

// collision response
void CPowerUpNoFriction::collisionResponse(	CSprite * collidedWith,
										float perCentOfMoveComplete, 
										CVector * polyintersectionPoint, 
										CVector * spriteOldPosition, 
										CVector * theVelocity) 
{
	if (ReplayManager::instance()->IsReplayingEndLevel())
	{
		return;
	}

	// perform specific responses to hitting different sprites
	switch(collidedWith->spriteType) // collapsing steps, enemeies etc
	{
		case SNEBULUS:
		{	
			/*	if the power-up is not already acting upon nebulus and nebulus is not 
				currently using a power-up */
			if (0 == actingUpon && (false == collidedWith->currentlyUsingPowerUp))
			{
				// call super class collision response
				CPowerUp::collisionResponse(collidedWith, perCentOfMoveComplete, 
											polyintersectionPoint, spriteOldPosition, 
											theVelocity); 
				
				// record Nebulus's original surface friction
				nebulusOriginalFrictionValue = actingUpon->surfaceFrictionConstant;
				
				// remove Nebulus's surface friction
				actingUpon->surfaceFrictionConstant = 0.0f;
			}
			break;
		}// end case SNEBULUS
        default: break;
	}// end switch
} 