//CSpriteEngine.h, header file for CSpriteEngine.cpp

//system includes
#include <vector>		// include this to use the vector class
//---------------

#ifndef _CSpriteEngine_h_
#define _CSpriteEngine_h_

//header files---------
#include "Rendering/CModel.h"
#ifdef _WINDOWS
#include "Rendering/CModelLoader.h"
#include "CSprite.h"
#include "CTowerSprite.h"
#include "CTowerStep.h"
#include "CNebulus.h"
#include "intersections.h"
#include "Math/CPlane.h" // describes a 3D plane (required for collision detection)
#include "CLift.h"
#include "CCollapsingStep.h"
#include "CTunnel.h"
#include "CSlippyStep.h"
#include "CTowerTop.h"
#include "CTowerBottom.h"
#include "CPowerUp.h"
#include "CPowerUpGrow.h"
#include "CPowerUpSpeed.h"
#include "CPowerUpNoFriction.h"
#include "CFireBall.h" // nebulus' fireball
#include "Camera/CCamera.h"
#include "CUpDownEnemy.h"
#include "CSideToSideEnemy.h"
#include "CBouncingSideToSideEnemy.h"
#include "CFlyAcrossEnemy.h"
#include "COctree.h"
#include "CFrustum.h"
#include "CExitScreen.h" // triangles when collided with end level
//---------------------

// forward declarations --	
class ReplayManager;
//------------------------

//defines--------------
/*	types of sprites which can be 
	found on the tower */ 
#define STEP						'='
#define BUFFER_STEP					'~'
#define TOWER_SEGMENT				'.'
#define DOOR_WAY					'#'
#define EXIT_DOOR					'X'
#define EXIT_DOOR_SCREEN			'^' // screen for exit door
#define SLIPPY_STEP_RIGHT			'>'
#define SLIPPY_STEP_LEFT			'<'
#define LIFT_BOTTOM					'T'
#define TUNNEL						'O' // the text file has referance to the associated model
#define TOWER_TOP					'*'
#define TOWER_BOTTOM				'+'
/*	the following sprite require a tower segment as well
	as the sprite representing themselves */
#define LIFT_TOP					'L'
#define COLLAPSING_STEP				'-'
#define NEBULUS						'N'
#define UP_DOWN_ENEMY				'U'
#define SIDE_TO_SIDE_ENEMY			'S'
#define BOUNCING_SIDE_TO_SIDE_ENEMY	'J'

// power-ups
enum power_ups    
{
   POWER_UP_SPEED_BOOST = 'a', 
   POWER_UP_DOUBLE_SIZE,
   POWER_UP_NO_FRICTION,
   POWER_UPS_AMT
}; 		

/*	types of sprites which do not get
	instantiated at specific locations
	around the tower */
#define RANDOM_POWER_UP		'R'
#define HELICOPTER			'H'
#define BATTLESHIP			'B'
#define FIREBALL			'F' // nebulus' fireball
#define FLY_ACCROSS_ENEMY	'@'
//---------------------

//external variables --
//---------------------

//global variables-----
//---------------------

class CSpriteEngine
{
	private:
//private variables------------
//-----------------------------

//private methods--------------
		// get the modelID from the legend information from the text file
		CModel* getModelFromLegend(int input);
		/*	test a moving object against the environment.
			find the first polygon hit then perform collision response */
		void movingObjectEnvironmentCollisionDetection(	CSprite *theSprite, 
														CVector * spriteOldPosition,
														CVector * velocity, 
														CPlane * prevPlaneStruck, 
														int reps);
		/*	find the closest triangle (from the collideeType sprites) to the bounding 
			sphere around the collider sprites current and previous position  */
		void findClosestTriangle(	CSprite * sprite, 
									CVector * boundingBoxCentre, 
									CVector * boundingBoxHalfWidths, 
									CVector * spriteOldPosition,
									CVector * velocity,
									GLfloat & perCentOfMoveComplete, 
									CVector & polyintersectionPoint, 
									CSprite **collidedWith, 
									CPlane &previousHitPlane, 
									int reps);

		// perform test against a single triangle
		void triangleTest(CSprite * sprite, CVector * boundingBoxCentre, 
								CVector * boundingBoxHalfWidths,  
								CVector * spriteOldPosition, CVector * velocity,
								GLfloat & perCentOfMoveComplete, 
								CVector & polyintersectionPoint, 
								CSprite **collidedWith, CPlane &previousHitPlane,
								int repititions, float	velocDotVeloc, float velocMagnitude, 
								CVector * verts, aTriangle *tri, CPlane *aPlane);
//-----------------------------
	public:
//public methods---------------
		CSpriteEngine(); // constructor
		~CSpriteEngine(); // destructor
		// loads all models using CModelLoader (returns success or failure)
		int loadModels(char** modelFiles, int numberOfModelsToLoad);
		// load all the sprites
		int loadSprites(int * nonTowerSpriteInfo, int nonTowerSpritesNo,
						char * towerSpriteInfo, int totalTowerObjects,
						int * legendInfo, int legendItemsNo); 
		void deleteCurrentModelsSprites(); // deletes current set of models
		// perform action on sprites based on key input
		void processKeyInput();
		void moveAll(int timeChange, GLfloat timeChangeInSeconds); // move all the sprites
		void collisionDetection(); // perform collision detection
		void drawAll(int timeChange, bool drawTransparentSprites); // draw all the sprites
		// create a random power-up from the power-ups available 
		CSprite* createRandomPowerUp();
		
		/*	set all the moving sprites back to their starting state,
			used when a level restarts after nebulus loses a life */
		void setMovingSpritesBack();
		void saveReplayUpdate(int frame);
#ifdef DETERMINISTIC_REPLAY_DEBUG
		void checkMovingSpritesPositions(ReplayManager& replay);
#endif
//-----------------------------

//public variables-------------
	CModel** models; // array of model pointers
	CSprite** sprites; // array of sprite pointers

	GLint	numberOfModels,
			numberOfSprites,
			*legendInformation, // array of legend information from text file
			amountOfLegendItems; // number of items in legend information array

	std::vector<CSprite*> movingSprites; // quick access to the moving sprites
	std::vector<CSprite*> boundingElipsoidSprites; // quick access to bounding ellipsoid sprites
	COctree *levelOctree; // pointer to the level octree member of CLevel

	CFrustum *theFrustum; // pointer to the singleton frustum
	CPowerUpManager *powerUpManager; // pointer to the singleton power-up manager
//-----------------------------
}; // class CSpriteEngine

#endif

#endif // _CSpriteEngine_h_