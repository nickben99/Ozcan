//CPowerUp.h - header for the power up class (includes the powerUp manager class)

//system includes------------
#include <Rendering/OpenGLInclude.h>
//---------------------------

#ifndef _CPowerUp_h_
#define _CPowerUp_h_

//header files---------
#include "CTowerStep.h"
#include "CHUD.h"
#include "Game/defines.h"
//---------------------

//defines--------------
#define POWER_UP_TIME_PERIOD 10.0f /* timer for power-ups which only last a finite 
									  length of time (10,000 milliseconds, 10 seconds) */
#define POWER_UP_ROTATE_SPEED PI*1.3f // the up down enemy rotates PI*1.3f every second	
//---------------------

//external definitions --
//-----------------------

// forward declerations ----
class CPowerUpManager;
// -------------------------

class CPowerUp : public CTowerStep
{
	private:
//-----private variables-----------
//---------------------------------

	public:
//----public variables--------------
		float powerUpTimer; // for timed power-ups
		CSprite* actingUpon; // the sprite which is being acted upon by this power-up

		char towerSegment; // identifies a tower segment

		bool drawThisFrame; // whether to draw the power-up in the current frame

		CHUD *theHUD; // pointer to the singleton HUD

		CPowerUpManager *powerUpManager; // pointer to the singleton power-up manager
//----------------------------------

//----public methods----------------------------------------
		// Constructor with model parameter
		CPowerUp(CModel * aModel, char towerSegmentIdentifier); 
		void reinitialise(); // initialisation
		// move the power-up
		virtual void move(int timeChange, GLfloat perCentOfSecond);
		void draw(int timeChange); // draw this sprite

		// virtual methods
		virtual ~CPowerUp(); // destructor
		// collision response
		virtual void collisionResponse(	CSprite * collidedWith,
										float perCentOfMoveComplete, 
										CVector * polyintersectionPoint, 
										CVector * spriteOldPosition, 
										CVector * velocity);

		void saveReplayUpdate(ReplayData&, bool);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CPowerUp

class CPowerUpManager
{
	private:
//---------private methods----------------------
		CPowerUpManager(){} // constructor
//----------------------------------------------

	public:
//---------public methods-----------
		// creates singleton powerUpManager instance
		static CPowerUpManager* instance() 
		{
			static CPowerUpManager CPowerUpManagerInstance; // create static camera instance once
			return &CPowerUpManagerInstance; // return the address of the camera instance
		}

		~CPowerUpManager() // destructor
		{
			removeAllPowerUpsFromList();  // clear the vector list
		} 

		void removeAllPowerUpsFromList( void)
		{
			powerUpsInLevel.clear(); // clear the vector list
		}

		// add a power up to the list (will be done once on power up creation)
		void addPowerUpToList(CPowerUp* powerUpToAdd)
		{
			powerUpsInLevel.push_back(powerUpToAdd);
		}

		int isPositionTaken(CVector *newPosition, CPowerUp* self)
		{
			// get the amount of power ups in the level
			int totalPowerUpsInLevel = (int)powerUpsInLevel.size();
			for (int pUp = 0; pUp < totalPowerUpsInLevel; pUp++)
			{
				if( (powerUpsInLevel[pUp] != self) && 
					(newPosition->fuzzyEquals(&powerUpsInLevel[pUp]->position)) ){
					return( true );} // position is taken and so can't be used
			}
			return( false ); // position is not taken and so can be used
		}
//----------------------------------

//---------public variables---------
		// a vector of all the power ups currently in the level
		std::vector<CPowerUp*> powerUpsInLevel;
//----------------------------------
};

#endif