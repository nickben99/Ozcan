//CPowerUp.cpp, the power up base class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CPowerUp.h"
#include "ReplayManager.h"
#include "Game/defines.h"
#include "Game/Globals.h"
//-------------------------

// constructor
CPowerUp::CPowerUp(CModel * aModel, char towerSegmentIdentifier)
	: CTowerStep(aModel)
{
	towerSegment = towerSegmentIdentifier; // set tower segment identifier
	actingUpon = 0; /*	set this to 0 here so on the first run of the initialise
							method below it will be 0, all other times it will 
							have a pointer to Nebulus.  If it was not set to 0 here the 
							first run of initialise would see actingUpon as not 0 but 
							not pointing to Nebulus, so would cause a crash */
	theHUD = CHUD::instance(); // get HUD singleton instance
	powerUpManager = CPowerUpManager::instance(); /*	get singleton power up 
														manager instance */
	reinitialise(); // initialise class
}

// initialise class
void CPowerUp::reinitialise()
{
	int randomPosition, rowsBelow[3];

	do
	{
		do
		{
			do
			{
				// get a random position on the tower
				randomPosition = RANDOM(0, theTower->totalTowerObjects-1);
	
				// get position three rows below on the tower
				rowsBelow[0] = (randomPosition - (theTower->towerColumns*1));
				rowsBelow[1] = (randomPosition - (theTower->towerColumns*2));
				rowsBelow[2] = (randomPosition - (theTower->towerColumns*3));

			/*	generate a new position if this position is 
				lower than the fourth row */
			} while (rowsBelow[2] < 0);
	
		/*	the power up should be generated on a tower segment, below the power up 
			should be two tower segments then a step, any other case will require a 
			new position to be generated */
		} while(	!(towerSegment == theTower->towerObjects[randomPosition].objectType && 
					towerSegment == theTower->towerObjects[rowsBelow[0]].objectType && 
					towerSegment == theTower->towerObjects[rowsBelow[1]].objectType && 
					
					(STEP == theTower->towerObjects[rowsBelow[2]].objectType || 
					SLIPPY_STEP_RIGHT == theTower->towerObjects[rowsBelow[2]].objectType || 
					SLIPPY_STEP_LEFT == theTower->towerObjects[rowsBelow[2]].objectType ||
					LIFT_BOTTOM == theTower->towerObjects[rowsBelow[2]].objectType || 
					LIFT_TOP == theTower->towerObjects[rowsBelow[2]].objectType)) );
		
		CTowerSprite::initialise(randomPosition); // initialise super class
		CTowerStep::initialise(randomPosition);

		// work out starting row of step beneath power-up
		int startingRow		= rowsBelow[2]/theTower->towerColumns;

		// position power-up 2.5f above top of the step beneath it
		position.setY(((float)startingRow*theTower->towerRowHeight)
						+(theTower->towerRowHeight*0.5f)+(BASE_MEASUREMENT_Y*2.5f));
	
		// create a vector to move the power-up 3/4 of way along the step below it
		CVector tempZTran(	0.0f, 
							0.0f,
							scale.z*(BASE_MEASUREMENT_Z*0.25f));
	
		// rotate by the power-ups rotation
		tempZTran = rotation.multiplyRotateVector(&tempZTran);
		// add the rotated vector onto the position vector of the power-up
		position += &tempZTran;

	/*	if this position is already occupied by another power up,
		generate a new position */
	} while (powerUpManager->isPositionTaken(&position, this));

	powerUpManager->addPowerUpToList(this);
	
	scale = CVector(1.0f, 1.0f, 1.0f); // model should be unscaled
	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

	dynamicProperty = MOVING; // set dynamic property
	collisionType = BOUNDING_SPHERE; // set collision type
	spriteType = SPOWER_UP; // set sprite type
	// if actingUpon has a pointer to nebulus, set Nebulus to not currently using a power-up
	if (0 != actingUpon)
	{
		actingUpon->currentlyUsingPowerUp = false;
		actingUpon = 0; // set variables to 0
	}
	powerUpTimer = 0.0f; //0; 
	drawThisFrame = true; // draw the power-up in the current frame

	theHUD->powerUpPerCentRemaining = 0.0f; /*	stops hud displaying that power up is still
												active after reinitialising */		

	oldPosition = position; /*	set these here and call calculateFinalMatrix so they do 
								not need to be called every frame if the sprite does not 
								move */
	calculateFinalMatrix();
}

// move the power-up
void CPowerUp::move(int timeChange, GLfloat perCentOfSecond)
{
	soundPlayedThisFrame = SOUNDS_COUNT;
	// create a matrix to rotate the sprite the required amount this frame
	CMatrix rotationThisFrame;
	rotationThisFrame.createYRotationMatrix(POWER_UP_ROTATE_SPEED*perCentOfSecond);

	rotation *= &rotationThisFrame; // add the new rotation onto the sprites rotation
	
	CSprite::move(timeChange, perCentOfSecond);
}

// destructor
CPowerUp::~CPowerUp()
{
}

// draw this sprite
void CPowerUp::draw(int timeChange)
{
	/*	if the power-up should be drawn this frame
		draw using the super class draw method */
	if (drawThisFrame){
		CSprite::draw(timeChange);}
} 

void CPowerUp::saveReplayUpdate(ReplayData& data, bool)
{
	if (ReplayManager::instance()->IsRecording())
	{
		data.drawThisFrame = drawThisFrame;
	}
	else
	{
		drawThisFrame = data.drawThisFrame;
	}
}

// collision response
void CPowerUp::collisionResponse(	CSprite * collidedWith,
									float, 
									CVector*, 
									CVector*, 
									CVector*) 
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
			// if the power-up is not already acting upon nebulus
			if (0 == actingUpon && !ReplayManager::instance()->IsReplayingEndLevel())
			{
				powerUpTimer = 0.0f;
				// set nebulus sprite which the power-up will be acting upon
				actingUpon = collidedWith;
				// don't draw the power-up anymore
				drawThisFrame = false;
				/*	nebulus is now using this power-up (to prevent two power-ups being 
					used at once) */
				actingUpon->currentlyUsingPowerUp = true;
				// set the hud with how much of power up is remaining
				theHUD->powerUpPerCentRemaining = 1.0f;

				soundPlayedThisFrame = SOUNDS_COLLECTPOWERUP;
				Globals::Instance().sound.PlaySound( SOUNDS_COLLECTPOWERUP, false ); // play power up collect sound
			}
			break;
		}// end case SNEBULUS
        default: break;
	}// end switch
} 









