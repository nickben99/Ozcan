//CPowerUpSpeed.cpp, the power up double speed class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CPowerUpSpeed.h"
#include "ReplayManager.h"
#include "Game/defines.h"

//-------------------------

// constructor
CPowerUpSpeed::CPowerUpSpeed(CModel * aModel, char towerSegmentIdentifier)
	: CPowerUp(aModel, towerSegmentIdentifier)
{
	nebulusOriginalThrustValue = // initialise values
	nebulusOriginalMaxMidAirSpeedValue = 0.0f;
}

// destructor
CPowerUpSpeed::~CPowerUpSpeed()
{
	CPowerUp::~CPowerUp();
}

// perform the power-ups action
void CPowerUpSpeed::move(int timeChange, GLfloat perCentOfSecond)
{	
	// call super class move to rotate the sprite
	CPowerUp::move(timeChange, perCentOfSecond);

	// if power-up has run out, lerp Nebulus's speed settings back down to normal
	if (actingUpon != 0)  
	{
		powerUpTimer += perCentOfSecond; // add on perCentOfSecond to timer

		theHUD->powerUpPerCentRemaining = // set HUD time remaining
			1.0f-(powerUpTimer/POWER_UP_TIME_PERIOD);

		if(powerUpTimer > POWER_UP_TIME_PERIOD)
		{
			// get percentage of speed reduction
			float perCentOfSpeedReduction = 
				(powerUpTimer-POWER_UP_TIME_PERIOD)/SPEED_REDUCTION_TIME_PERIOD;
			
			bool powerUpFinished = false;
			
			// if perCentOfSpeedReduction is greater than 1.0f
			if (perCentOfSpeedReduction > 1.0f){
				powerUpFinished = true; // power-up is finished
				perCentOfSpeedReduction = 1.0f;} // cap speed reduction at 1.0f
	
			// set nebulus values
			actingUpon->forwardThrust = ( (nebulusOriginalThrustValue * 2.0f) - 
				(perCentOfSpeedReduction*nebulusOriginalThrustValue) );

			actingUpon->midAirGroundPlaneMaxSpeed = 
				( (nebulusOriginalMaxMidAirSpeedValue * 2.0f) - 
				(perCentOfSpeedReduction*nebulusOriginalMaxMidAirSpeedValue) );
	
			// power-up is finished so reinitialise in a random location
			if (powerUpFinished){
				CPowerUp::reinitialise();}
		}
	}
}

// draw this sprite
void CPowerUpSpeed::draw(int timeChange)
{
	CPowerUp::draw(timeChange);
} 

// collision response
void CPowerUpSpeed::collisionResponse(	CSprite * collidedWith,
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
				
				// record original values
				nebulusOriginalThrustValue = actingUpon->forwardThrust;
				nebulusOriginalMaxMidAirSpeedValue = actingUpon->midAirGroundPlaneMaxSpeed;	
				
				// double Nebulus's velocity
				actingUpon->forwardThrust *= 2.0f;
				actingUpon->midAirGroundPlaneMaxSpeed *= 2.0f;
			}
			break;
		}// end case SNEBULUS
	}// end switch
} 