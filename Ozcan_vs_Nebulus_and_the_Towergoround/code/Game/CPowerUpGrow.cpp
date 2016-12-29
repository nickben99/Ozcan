//CPowerUpGrow.cpp, the power up double size class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CPowerUpGrow.h"
#include "ReplayManager.h"
#include "Game/defines.h"

//-------------------------

// constructor
CPowerUpGrow::CPowerUpGrow(CModel * aModel, char towerSegmentIdentifier)
	: CPowerUp(aModel, towerSegmentIdentifier)
{
}

// destructor
CPowerUpGrow::~CPowerUpGrow()
{
}

// perform the power-ups action
void CPowerUpGrow::move(int timeChange, GLfloat perCentOfSecond)
{
	// call super class move to rotate the sprite
	CPowerUp::move(timeChange, perCentOfSecond);

	// if the power-up has a pointer to the nebulus sprite to act upon it
	if (actingUpon != 0) 	
	{
		powerUpTimer += perCentOfSecond; // add on perCentOfSecond to timer

		theHUD->powerUpPerCentRemaining = // set hud with time remaining
			1.0f-(powerUpTimer/POWER_UP_TIME_PERIOD);
		
		// if within the grow period, get the percentage of the grow 
		float perCentOfGrow = powerUpTimer/GROW_TIME_PERIOD;

		// can't go over 1.0f
		if (perCentOfGrow > 1.0f){
			perCentOfGrow = 1.0f;}
		
		// nebulus will appear to double in size 
		actingUpon->drawScale = 
			CVector(1.0f+perCentOfGrow, 1.0f+perCentOfGrow, 1.0f+perCentOfGrow);

		// scale the nebulus animation speed in line with the size change
		actingUpon->topGroundSpeedFramePerSecond = 
			(originaltopGroundSpeedFramePerSecond - 
				perCentOfGrow*(originaltopGroundSpeedFramePerSecond*0.25f));

		// if power-up has run out
		if (powerUpTimer > POWER_UP_TIME_PERIOD)
		{
			// get percentage of shrink
			float perCentOfShrink = 
				(powerUpTimer-POWER_UP_TIME_PERIOD)/GROW_TIME_PERIOD;
			
			bool powerUpFinished = false;
			
			// if perCentOfShrink is greater than 1.0f
			if (perCentOfShrink > 1.0f){
				powerUpFinished = true; // power-up is finished
				perCentOfShrink = 1.0f;} // cap shrink at 1.0f
	
			// Nebulus's scale for collision detection will shrink
			actingUpon->scale = 
				CVector(2.0f-perCentOfShrink, 2.0f-perCentOfShrink, 2.0f-perCentOfShrink);
			// set current bounding box and elipsoid radius vectors based on scale
			actingUpon->scaledBoundingBox = actingUpon->boundingBox * &actingUpon->scale;
			actingUpon->scaledHalfBoundingBox = actingUpon->scaledBoundingBox * 0.5f;
			actingUpon->scaledElipsoidRadiusVector = actingUpon->elipsoidRadiusVector * &actingUpon->scale;
			// Nebulus's scale for drawing will also shrink
			actingUpon->drawScale = actingUpon->scale;

			// scale the nebulus animation speed in line with the size change
			actingUpon->topGroundSpeedFramePerSecond = 
				(originaltopGroundSpeedFramePerSecond - 
					((1.0f-perCentOfShrink)*(originaltopGroundSpeedFramePerSecond*0.25f)));

			// power-up is finished so reinitialise in a random location
			if (powerUpFinished){
				CPowerUp::reinitialise();}
		}
	}
	
}

// draw this sprite
void CPowerUpGrow::draw(int timeChange)
{
	CPowerUp::draw(timeChange);
} 

// collision response
void CPowerUpGrow::collisionResponse(	CSprite * collidedWith,
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
				// set nebulus's scale for collision detection to double size
				actingUpon->scale = CVector(2.0f, 2.0f, 2.0f);
				// set current bounding box and elipsoid radius vectors based on scale
				actingUpon->scaledBoundingBox = actingUpon->boundingBox * &actingUpon->scale;
				actingUpon->scaledHalfBoundingBox = actingUpon->scaledBoundingBox * 0.5f;
				actingUpon->scaledElipsoidRadiusVector = actingUpon->elipsoidRadiusVector * &actingUpon->scale;	
			
				// get Nebulus' top ground Speed frames per second
				originaltopGroundSpeedFramePerSecond = (int)actingUpon->topGroundSpeedFramePerSecond;
			}
			break;
		}// end case SNEBULUS
        default: break;
	}// end switch
} 