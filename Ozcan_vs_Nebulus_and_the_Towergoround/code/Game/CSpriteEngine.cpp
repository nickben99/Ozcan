//CSpriteEngine.cpp, the CSpriteEngine class

//system includes-------
//----------------------

//header files----------
#include "CSpriteEngine.h"
#include "ReplayManager.h"
using namespace std; /* needs to be declared after header include as vector is declared
						in header */
//----------------------

//defines---------------

#include "assert.h"
//----------------------

CSpriteEngine::CSpriteEngine()
{
	numberOfModels = 0;
	numberOfSprites = 0;
	legendInformation = 0;
	models = 0;
	sprites = 0;
	powerUpManager = CPowerUpManager::instance(); // get pointer to the power up manager
}

CSpriteEngine::~CSpriteEngine()
{
	deleteCurrentModelsSprites();
}

// deletes current set of models
void CSpriteEngine::deleteCurrentModelsSprites()
{
	if (legendInformation) /*	the memory allocation for this is done externally
								to this class but must be deleted here */
	{
		delete[] legendInformation;
		legendInformation = 0;
	}

	if (sprites) // check the sprites dynamic array is not null
	{
		// delete each element in the sprites array
		for (int i = 0; i < numberOfSprites; i++)
		{
			if (sprites[i])
			{	/*	if this is the nebulus sprite, the singleton pointer must be destroyed 
					using the static method, if not a normal delete operation is all that is 
					required */
				if (sprites[i]->spriteType != SNEBULUS){
					delete sprites[i];}
				else{
					CNebulus::destroyInstance();} // static method to delete nebulus
				sprites[i] = 0;
			} // delete all the sprites
		}
	
		// reclaim memory allocated to sprites array
		delete[]sprites;
		sprites = 0;
		numberOfSprites = 0;
	} // end if sprites
	
	if (models) // check the models dynamic array is not null
	{
		// delete each element in the models array
		for (int i = 0; i < numberOfModels; i++)
		{
			if (models[i])
			{
				delete models[i];
				models[i] = 0;
			} // delete all the models
		}

		// reclaim memory allocated to models array
		delete[]models;
		models = 0;
		numberOfModels = 0;
	} // end if models	

	movingSprites.clear(); // clear the vector lists
	boundingElipsoidSprites.clear();

	// remove all this levels power ups from the power up manager
	powerUpManager->removeAllPowerUpsFromList(); 
}

int CSpriteEngine::loadModels(char** modelFiles, int numberOfModelsToLoad)
{
	CModel * aModel = 0; // temp model pointer

	// delete anything in the CSpriteEngines array of models
	if(models)delete[]models; 
	
	// create array of models equal to the number of files being loaded
	models=new CModel*[numberOfModelsToLoad]; 
	
	if(!models){ 
		return false;} // memory allocation failed so return false
		
	for (int model = 0; model < numberOfModelsToLoad; model++){
		models[model] = 0;} // initialise to null
	
	// load all the models
	for (int model = 0; model < numberOfModelsToLoad; model++)
	{
		if (aModel){ // if it points to a model
			aModel = 0;} // set to null 

		if (CModelLoader::load(modelFiles[model], &aModel)) 
		{ // succesful load
			models[model] = aModel; // put model in the dynamic array
			numberOfModels++; // increment the number of models loaded
		}
		else { // loading problem
			return(false); }
	}
	
	if (aModel){ // if it points to a model
		aModel = 0;} // set to null 

	return(true); // success
}

// load all this levels sprites
int CSpriteEngine::loadSprites(int * nonTowerSpriteInfo, int nonTowerSpritesNo,
							   char * towerSpriteInfo, int totalTowerObjects,
							   int * legendInfo, int legendItemsNo)
{
	legendInformation = legendInfo,
	amountOfLegendItems = legendItemsNo;
	
	int noOfTowerSprites = totalTowerObjects,
		totalNonTowerSpritesNo = 0;
	
	/*	find all tower sprites which will also require a tower segment to be rendered,
		will be required for collapsing steps, tower enemies, tunnels for door ways etc */
	int extraSegmentsRequired = 0;

	for (int towerSprite = 0; towerSprite < noOfTowerSprites; towerSprite++){
		if (DOOR_WAY == towerSpriteInfo[towerSprite]){
			extraSegmentsRequired++;}
	}
	// one tunnel is required for each set of two DOOR_WAYS, therefore divide by two
	extraSegmentsRequired /= 2;

	for (int towerSprite = 0; towerSprite < noOfTowerSprites; towerSprite++){
		if (COLLAPSING_STEP == towerSpriteInfo[towerSprite] || 
			LIFT_BOTTOM == towerSpriteInfo[towerSprite] || 
			NEBULUS == towerSpriteInfo[towerSprite] || 
			UP_DOWN_ENEMY == towerSpriteInfo[towerSprite] || 
			SIDE_TO_SIDE_ENEMY == towerSpriteInfo[towerSprite] || 
			BOUNCING_SIDE_TO_SIDE_ENEMY == towerSpriteInfo[towerSprite] ||  
			BUFFER_STEP == towerSpriteInfo[towerSprite] || 
			EXIT_DOOR_SCREEN == towerSpriteInfo[towerSprite]){
			extraSegmentsRequired++;}

		if (BOUNCING_SIDE_TO_SIDE_ENEMY == towerSpriteInfo[towerSprite]){
			extraSegmentsRequired += 2;} /* a tower segment and buffer step is required for
											the bouncing side to side enemy */
	}
	noOfTowerSprites += extraSegmentsRequired;

	// get the amount of non-tower sprites
	for (int nonTowerSprite = 0; nonTowerSprite < nonTowerSpritesNo; nonTowerSprite++){
		totalNonTowerSpritesNo += nonTowerSpriteInfo[(nonTowerSprite*2)+1];}

	numberOfSprites = noOfTowerSprites+totalNonTowerSpritesNo; // set sprite number
	sprites = new CSprite*[numberOfSprites]; // allocate memory for sprites

	if (!sprites){
		return(false);} // memory allocation failed

	for (int sprite = 0; sprite < numberOfSprites; sprite++){
		sprites[sprite] = 0;} // set all to null initially

	int extraSegmentCounter = 0; // to offset the extra tower segment sprites required
	bool tunnelRequired = true; // every other door way will require a tunnel

	// parse tower objects to initialise the appropriate sprites
	for (int sprite = 0; sprite < totalTowerObjects; sprite++)
	{
		switch(towerSpriteInfo[sprite])
		{
			case TOWER_SEGMENT:
				sprites[sprite] = // tower segment is just a simple tower sprite
					new CTowerSprite(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				break;
			case LIFT_TOP:
				sprites[sprite] = // tower segment is just a simple tower sprite
					new CTowerStep(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				break;
			case BUFFER_STEP:
				sprites[sprite] = // tower step which is positioned appropriatly
					new CBufferStep(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				/*	the tower segment sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerSprite(getModelFromLegend(TOWER_SEGMENT), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			case STEP:
				sprites[sprite] = // tower step which is positioned appropriatly
					new CTowerStep(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				break;
			case NEBULUS:
				sprites[sprite] = // the Nebulus singleton class
					CNebulus::instance(getModelFromLegend(towerSpriteInfo[sprite]), sprite, NEBULUS_STARTING_LIVES);
				/*	the tower segment sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerSprite(getModelFromLegend(TOWER_SEGMENT), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			case COLLAPSING_STEP:
				sprites[sprite] = // the collapsing step sprite
					new CCollapsingStep(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				/*	the tower segment sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerSprite(getModelFromLegend(TOWER_SEGMENT), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			case DOOR_WAY:
				sprites[sprite] = 
					new CTowerSprite(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				if (tunnelRequired) // every other door way will require a tunnel
				{
					tunnelRequired = false; // tunnel not required for next door way
					/*	the tunnel sprite placed after the tower sprites but before the
						non-tower sprites */
					sprites[totalTowerObjects + extraSegmentCounter] = 
						new CTunnel(getModelFromLegend(TUNNEL), sprite);
					extraSegmentCounter++; // keep track of the extra segments found so far
				}// end if tunnelRequired
				else{
					tunnelRequired = true;} // tunnel is required for the next door way
				break;
			case EXIT_DOOR:
				sprites[sprite] = 
					new CTowerSprite(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				/*	the exit door screen placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CExitScreen(getModelFromLegend(EXIT_DOOR_SCREEN), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			case SLIPPY_STEP_RIGHT:
			case SLIPPY_STEP_LEFT:
				sprites[sprite] = // the collapsing step sprite
					new CSlippyStep(getModelFromLegend(towerSpriteInfo[sprite]), sprite, 
					(SLIPPY_STEP_LEFT == towerSpriteInfo[sprite]) ? true : false);
				break;
			case LIFT_BOTTOM:
				sprites[sprite] = 
					new CLift(0, sprite, LIFT_TOP);
				/*	the lift bottom sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerStep(getModelFromLegend(towerSpriteInfo[sprite]), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			case UP_DOWN_ENEMY:	
				sprites[sprite] = 
					new CUpDownEnemy(getModelFromLegend(UP_DOWN_ENEMY), sprite);
				/*	the tower segment sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerSprite(getModelFromLegend(TOWER_SEGMENT), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			case BOUNCING_SIDE_TO_SIDE_ENEMY:
				sprites[sprite] = 
					new CBouncingSideToSideEnemy(getModelFromLegend(BOUNCING_SIDE_TO_SIDE_ENEMY), sprite);
				/*	the tower segment sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerSprite(getModelFromLegend(TOWER_SEGMENT), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				/*	the buffer step sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerStep(getModelFromLegend(BUFFER_STEP), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			case SIDE_TO_SIDE_ENEMY:
				sprites[sprite] = 
					new CSideToSideEnemy(getModelFromLegend(SIDE_TO_SIDE_ENEMY), sprite);
				/*	the tower segment sprite placed after the tower sprites but before the
					non-tower sprites */
				sprites[totalTowerObjects + extraSegmentCounter] = 
					new CTowerSprite(getModelFromLegend(TOWER_SEGMENT), sprite);
				extraSegmentCounter++; // keep track of the extra segments found so far
				break;
			default: 
				sprites[sprite] = 0;
				break;
		}// end switch towerSpriteInfo
	}// end for tower objects

	// initialise sprite objects which are external to the tower
	int indexIntoSpritesArray = noOfTowerSprites; 

	//	go through the non tower sprite info array sent as a parameter
	for (int nonTowerSprite = 0; nonTowerSprite < nonTowerSpritesNo; nonTowerSprite++)
	{	// go through loading all the sprites required of this type
		for (int amtToLoad = 0; amtToLoad < nonTowerSpriteInfo[(nonTowerSprite*2)+1]; 
																				amtToLoad++)
		{
			switch(nonTowerSpriteInfo[nonTowerSprite*2])
			{
				case RANDOM_POWER_UP:
					// add a randomly generated power-up to the array
					sprites[indexIntoSpritesArray] = createRandomPowerUp();
					// add the power up to the power up managers array
					indexIntoSpritesArray++;
					break;
				case HELICOPTER:
					/*	initialise sprite, send modelID which is the contiguous array 
						element as paramter */
					sprites[indexIntoSpritesArray] = 
						new CSprite(getModelFromLegend(nonTowerSpriteInfo[nonTowerSprite*2]));
					indexIntoSpritesArray++;
					break;
				case BATTLESHIP:
					sprites[indexIntoSpritesArray] = 
						new CSprite(getModelFromLegend(nonTowerSpriteInfo[nonTowerSprite*2]));
					indexIntoSpritesArray++;
					break;
				case FIREBALL: // nebulus' fireball
					sprites[indexIntoSpritesArray] = 
						new CFireBall(getModelFromLegend(nonTowerSpriteInfo[nonTowerSprite*2]));
					indexIntoSpritesArray++;
					break;
				case TOWER_TOP: // top of the tower
					sprites[indexIntoSpritesArray] = 
						new CTowerTop(getModelFromLegend(nonTowerSpriteInfo[nonTowerSprite*2]));
					indexIntoSpritesArray++;
					break;
				case TOWER_BOTTOM: // bottom of the tower
					sprites[indexIntoSpritesArray] = 
						new CTowerBottom(getModelFromLegend(nonTowerSpriteInfo[nonTowerSprite*2]));
					indexIntoSpritesArray++;
					break;
				case FLY_ACCROSS_ENEMY:
					sprites[indexIntoSpritesArray] = 
						new CFlyAcrossEnemy(getModelFromLegend(nonTowerSpriteInfo[nonTowerSprite*2]));
					indexIntoSpritesArray++;
					break;
				default: 
					sprites[indexIntoSpritesArray] = 0;
					break;
			}// end switch nonTowerSpriteInfo
		}// end for sprite amtToLoad
	}// end for nonTowerSprite

	// fill the various quick access lists
	for (int sprite = 0; sprite < numberOfSprites; sprite++)
	{	
		if (!sprites[sprite]){ // if null
			continue;}

		CSprite *aSprite = sprites[sprite]; // referance variable
		// fill the vector of moving sprites (to provide quick access to them)
		if ((MOVING == aSprite->dynamicProperty) || (MOVING_ENVIRONMENT == aSprite->dynamicProperty))
		{
			movingSprites.push_back(aSprite);
		}

		// fill the vector of bounding ellipsoid sprites required for collision detection
		if ((MOVING == aSprite->dynamicProperty) && (BOUNDING_ELIPSOID == aSprite->collisionType))
		{
			boundingElipsoidSprites.push_back(aSprite);
		}
	}// end for sprite

	// get singleton frustum pointer
	theFrustum = CFrustum::instance();

	return(true);
}

//	set all the moving sprites back to their starting state, used when a level restarts after nebulus loses a life
void CSpriteEngine::setMovingSpritesBack()
{
	CPowerUpManager::instance()->removeAllPowerUpsFromList();

	int totalMovingSprites = (int)movingSprites.size();
	for (int sprite = 0; sprite < totalMovingSprites; sprite++)
	{
		movingSprites[sprite]->reinitialise();
		movingSprites[sprite]->ReplayInit();
	}
}

void CSpriteEngine::saveReplayUpdate(int frame)
{
	ReplayManager::instance()->PreSpriteUpdate();

	int totalMovingSprites = (int)movingSprites.size();
	for (int sprite = 0; sprite < totalMovingSprites; sprite++)
	{
		movingSprites[sprite]->saveReplayUpdate(frame);
	}

	ReplayManager::instance()->PostSpriteUpdate();
}

#ifdef DETERMINISTIC_REPLAY_DEBUG
void CSpriteEngine::checkMovingSpritesPositions(ReplayManager& replay)
{
	int totalMovingSprites = movingSprites.size();
	for (int sprite = 0; sprite < totalMovingSprites; sprite++)
	{
		replay.VectorResult(movingSprites[sprite]->position);
		replay.MatrixResult(movingSprites[sprite]->rotation);
	}
}
#endif

// get the modelID from the legend information from the text file
CModel* CSpriteEngine::getModelFromLegend(int input)
{
	char spriteID = (char)input;
	/*	go through the legend information, find the spritID
		and the accompanying modelID */
	for (int model = 0; model < amountOfLegendItems; model++)
	{
		if (legendInformation[model*2] == (int)spriteID){
			return(models[legendInformation[(model*2)+1]]);} // model found
	}
	return(0); // no model found (shouldn't get here)
}

// create a random power-up from the power-ups available 
CSprite* CSpriteEngine::createRandomPowerUp()
{
	// randoly select a power-up
	int randomPowerUp = RANDOM(POWER_UP_SPEED_BOOST, POWER_UPS_AMT-1);
	switch(randomPowerUp) // create appropriate power-up based on the random number
	{
		case POWER_UP_SPEED_BOOST:
			return(new CPowerUpSpeed(getModelFromLegend(randomPowerUp), TOWER_SEGMENT));
		case POWER_UP_DOUBLE_SIZE:
			return(new CPowerUpGrow(getModelFromLegend(randomPowerUp), TOWER_SEGMENT));
		case POWER_UP_NO_FRICTION:
			return(new CPowerUpNoFriction(getModelFromLegend(randomPowerUp), TOWER_SEGMENT));
		default: 
			return(0);
			break;
	}
}

// perform action on sprites based on key input
void CSpriteEngine::processKeyInput()
{
#if _DEBUG
	if (CCamera::instance()->IsDebugCamActive() && !CCamera::instance()->IsDebugCamPositionLocked())
	{
		return;
	}
#endif

	int totalMovingSprites = (int)movingSprites.size();

	for (int i = 0; i < totalMovingSprites; i++)
	{ //  go through the moving objects
		CSprite &aSprite = *movingSprites[i];
		switch(aSprite.spriteType)
		{
			case SNEBULUS:
#ifdef DETERMINISTIC_REPLAY_DEBUG
				ReplayManager::instance()->VectorResult(CCamera::instance()->GetGameCameraControlForward());
				ReplayManager::instance()->VectorResult(CCamera::instance()->GetGameCameraControlRight());
#endif
				aSprite.keyInput(CCamera::instance()->GetGameCameraControlForward(), CCamera::instance()->GetGameCameraControlRight());
				break;
			case SFIRE_BALL:
				aSprite.keyInput();
				break;
            default: break;
		}// end switch spriteType
	}
}

// move all the sprites
void CSpriteEngine::moveAll(int timeChange, GLfloat timeChangeInSeconds) 
{
	int totalMovingSprites = (int)movingSprites.size();

	for (int i = 0; i < totalMovingSprites; i++){ //  go through the moving objects
		movingSprites[i]->move(timeChange, timeChangeInSeconds);} // move the sprite
}

// perform collision detection
void CSpriteEngine::collisionDetection()
{
	/*	test all MOVING objects with a BOUNDING_ELIPSOID 
		against the environment */
	int amountOfSprites = (int)boundingElipsoidSprites.size();
	for (int i = 0; i < amountOfSprites; i++) //  go through bounding elipsoid objects
	{	
		// referance variable for readability and quicker execution
		CSprite &aSprite = *boundingElipsoidSprites[i];
		// if its active
		if (aSprite.active)
		{ 
			
			// static variables to avoid initiating them every time the function is called
			static CMatrix scale;
			static CVector velocity, spriteOldPosition,	spritePosition;

			// create a scaling matrix to convert into ellipsoid space
			scale.creatScalingMatrix(	aSprite.scaledElipsoidRadiusVector.x, 
										aSprite.scaledElipsoidRadiusVector.y, 
										aSprite.scaledElipsoidRadiusVector.z);

			// get previous and new sprite position
			spriteOldPosition = aSprite.oldPosition;
			spritePosition = aSprite.position;
			// get vector from previous to current position
			velocity = (aSprite.position - &aSprite.oldPosition);
	
			// convert position and velocity to elipsoid space
			spriteOldPosition = 
				(scale.getInverseScalingMatrix()).multiplyVector(&spriteOldPosition);
		
			velocity  = 
				(scale.getInverseScalingMatrix()).multiplyVector(&velocity);
			
			// do collision detection process
			CPlane p;
			movingObjectEnvironmentCollisionDetection(&aSprite, &spriteOldPosition, &velocity, 
														&p, 0);

			// add on velocity
			spriteOldPosition += &velocity;
			// convert out of elipsoid space
			spriteOldPosition = 
				scale.multiplyVector(&spriteOldPosition);

			// set position
			aSprite.position = spriteOldPosition;
			aSprite.calculateFinalMatrix(); // calculate the objects final matrix
		}
	}
		
	int totalMovingSprites = (int)movingSprites.size();
	// test all the moving objects and against each other
	for (int i = 0; i < totalMovingSprites; i++) //  go through the moving objects
	{	// if MOVING object is active 
		if (movingSprites[i]->active && (MOVING == movingSprites[i]->dynamicProperty))
		{
			for (int j = i+1; j < totalMovingSprites; j++) //  go through moving objects
			{	/*	if MOVING object (j) and both are active (one could be made innactive 
					in collision detection) */
				if (movingSprites[i]->active && movingSprites[j]->active && 
					(MOVING == movingSprites[j]->dynamicProperty))
				{
					CSprite		*aSprite1 = movingSprites[i],
								*aSprite2 = movingSprites[j];

					// set to true when the intersection has been evaluated
					bool collisionTestEvaluated = false; 
					
					do // do until the collision has been evaluated
					{	// switch on collision type; capsule, sphere, elipsoid etc
						switch (aSprite1->collisionType)
						{
							case BOUNDING_CAPSULE: 
							{
								switch (aSprite2->collisionType)
								{
									case BOUNDING_SPHERE:
									case BOUNDING_ELIPSOID: // considered a sphere here
									{
										// see if sphere and capsule have collided
										if (sphereAxisAlignedCapsuleIntersection(
												&aSprite1->position, 
												aSprite1->boundingSphereRadius,
												aSprite1->height, &aSprite2->position, 
												aSprite2->scaledElipsoidRadiusVector.x))
										{	// if their was a collision call collision response
											CVector v;
											aSprite2->collisionResponse(aSprite1, 0.0f, &v, &v, &v);
											aSprite1->collisionResponse(aSprite2, 0.0f, &v, &v, &v);
										}
										collisionTestEvaluated = true; // test has been evaluated
										break;
									}// end case BOUNDING_ELIPSOID
									case BOUNDING_CAPSULE:
									{
										collisionTestEvaluated = true;
										break;
									}// end case BOUNDING_CAPSULE
                                    default: break;
								}// end switch sprites[sprite2]->collisionType
								break;
							}// end case BOUNDING_CAPSULE
							case BOUNDING_SPHERE:
							case BOUNDING_ELIPSOID:
							{
								switch (aSprite2->collisionType)
								{
									case BOUNDING_SPHERE:
									case BOUNDING_ELIPSOID: // considered a sphere here
									{
										float hitTime; // referance variable for below method call
										// see if spheres have hit each other this frame
										if ( sphereSphereIntersectionWithinVelocity(aSprite1->oldPosition, 
											aSprite1->scaledElipsoidRadiusVector.x, 
											(aSprite1->position - aSprite1->oldPosition),
											aSprite2->oldPosition, 
											aSprite2->scaledElipsoidRadiusVector.x, 
											(aSprite2->position - aSprite2->oldPosition), hitTime))
										{	// call response methods
											CVector v;
											aSprite1->collisionResponse(aSprite2, 0.0f, &v, &v, &v);
											aSprite2->collisionResponse(aSprite1, 0.0f, &v, &v, &v);
										}
										collisionTestEvaluated = true; // test has been evaluated
										break;
									}
                                    default: break;
								}// end switch sprites[sprite2]->collisionType
								break;
							}// end case BOUNDING_ELIPSOID
                            default: break;
						}// end switch sprites[sprite1]->collisionType
						/*	swap pointers and do again if collision has still 
							not been evaluated */
						CSprite *temp = aSprite2;
						aSprite2 = aSprite1;
						aSprite1 = temp;

					} while (!collisionTestEvaluated); 
				}// end if moving object found
			}// end for j
		}// end if moving object found
	}// end for i
}

/*	test a moving object against the environment.
	find the first polygon hit then perform collision response */
void CSpriteEngine::movingObjectEnvironmentCollisionDetection(CSprite *theSprite, 
															  CVector *spriteOldPosition,
															  CVector *velocity, 
															  CPlane *prevPlaneStruck,
															  int reps)
{
	// the same plane cannot be hit on consecutive runs, this stops this occuring
	static CPlane previousHitPlane;
	previousHitPlane = *prevPlaneStruck;

	/*	used to restrict the amount of times this method runs, so as to prvent a crash 
		if a situation occurs where the same planes are continually being struck */
	int repititions = reps; 

	// don't perform collision checks if the distance travelled is so small
	if ((*velocity * &theSprite->scaledElipsoidRadiusVector).getMagnitude() 
		< 0.000001f){
		return;}       

	// get bounding box around previous and current sprite positions
	CVector worldCoordOldPos	= (*spriteOldPosition * &theSprite->scaledElipsoidRadiusVector),
			worldCoordVeloc		= (*velocity * &theSprite->scaledElipsoidRadiusVector), 
			tempHalfVeloc		= worldCoordVeloc * 0.5f, 
			boundingBoxCentre	= (worldCoordOldPos + &tempHalfVeloc);

	tempHalfVeloc.makeVectorPositive();
	CVector boundingBoxHalfWidths = tempHalfVeloc + &(theSprite->scaledHalfBoundingBox);

	// see which octree end nodes the sprite is in
	theSprite->endNodesOccupying.clear(); // clear its current list
	levelOctree->putSpriteIntoEndNode(&boundingBoxHalfWidths, &boundingBoxCentre, theSprite->endNodesOccupying);

	// referance variables to be sent to methods below
	GLfloat perCentOfMoveComplete = 2.0f; // initialise
	CVector polyintersectionPoint;
	CSprite *spriteCollidedWith;
	
	//	find the first triangle hit from the static or moving environment 
	findClosestTriangle(theSprite, &boundingBoxCentre, 
						&boundingBoxHalfWidths, spriteOldPosition, velocity, 
						perCentOfMoveComplete, polyintersectionPoint, &spriteCollidedWith, 
						previousHitPlane, repititions);

	// if their have been collisions
	if (perCentOfMoveComplete < 2.0f)
	{
		// perform the collision response for the two objects involved in the collision
		theSprite->collisionResponse(spriteCollidedWith, perCentOfMoveComplete, 
									&polyintersectionPoint, spriteOldPosition, 
									velocity);
		spriteCollidedWith->collisionResponse(theSprite, perCentOfMoveComplete, 
									&polyintersectionPoint, spriteOldPosition, 
									velocity);
		// recursivly call this method to check for any new collisions
		movingObjectEnvironmentCollisionDetection(theSprite, spriteOldPosition, velocity, 
												&previousHitPlane, ++repititions);
	}
}

/*	find the closest triangle (from the collideeType sprites) to the bounding 
	sphere around the collider sprites current and previous position  */
void CSpriteEngine::findClosestTriangle(CSprite * sprite, 
										CVector * boundingBoxCentre, 
										CVector * boundingBoxHalfWidths,  
										CVector * spriteOldPosition,
										CVector * velocity,
										GLfloat & perCentOfMoveComplete, 
										CVector & polyintersectionPoint, 
										CSprite **collidedWith, 
										CPlane &previousHitPlane,
										int reps)
{
	CPlane aPlane; // to record the plane struck
	int repititions = reps; // to record the number of times this method has run
	// compute values below, which may be used multiple times
	float	velocDotVeloc = velocity->dotProduct(velocity), 
			velocMagnitude = velocity->getMagnitude();

	// go through the octree end nodes which the sprite occupys
	int totalEndNodesOccupying = (int)sprite->endNodesOccupying.size();

	// set all triangles in all end nodes occupying to not evaluated yet
	for (int endNode = 0; endNode < totalEndNodesOccupying; endNode++)
	{	// referance for speed and readability
		COctree &aNode = *sprite->endNodesOccupying[endNode]; 
		int totalNodeTriangles = (int)aNode.nodeTriangles.size();

		for (int nodeTriangle = 0; nodeTriangle < totalNodeTriangles; nodeTriangle++){
			aNode.nodeTriangles[nodeTriangle]->collisionTested = false;}
	}

	for (int endNode = 0; endNode < totalEndNodesOccupying; endNode++)
	{	// referance for speed and readability
		COctree &aNode = *sprite->endNodesOccupying[endNode]; 
		int totalNodeTriangles = (int)aNode.nodeTriangles.size();

		for (int nodeTriangle = 0; nodeTriangle < totalNodeTriangles; nodeTriangle++)
		{	// referance for speed and readability
			aTriangle &tri = *aNode.nodeTriangles[nodeTriangle]; 

			if (tri.collisionTested){ // has the triangle been tested
				continue;}

			tri.collisionTested = true; // dont test this triangle again

			CVector verts[] = {	CVector() = tri.vertices[0], CVector() = tri.vertices[1],
								CVector() = tri.vertices[2]};

			triangleTest(sprite, boundingBoxCentre, 
								boundingBoxHalfWidths,  
								spriteOldPosition, velocity,
								perCentOfMoveComplete, 
								polyintersectionPoint, 
								collidedWith, previousHitPlane,
								repititions, velocDotVeloc, velocMagnitude, 
								&verts[0], &tri, &aPlane);
		}
	}

	int totalMovingSprites = (int)movingSprites.size(); // get amt of moving sprites
	// go through all the sprites to get all the triangles
	for (int i = 0; i < totalMovingSprites; i++)
	{	
		CSprite& aSprite = *movingSprites[i]; // set referance variable to sprite
		
		// continue if not moving environment
		if (aSprite.dynamicProperty != MOVING_ENVIRONMENT){ 
			continue;}
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//		ReplayManager::instance()->IntResult(aSprite.dynamicProperty);
//#endif

		// continue if sprite and moving_environment don't collide
		if (!sphereAxisAlignedBoxIntersection(boundingBoxCentre, boundingBoxHalfWidths, 
					&aSprite.position, aSprite.boundingBoxRadius)){
			continue;}

		vector<CVector> meshVertices; /*	vector to store the sprites models 
											vertices once transformed by the sprite
											final matrix */

		// go through all the sprite models meshes
		for (int aMesh = 0; aMesh < aSprite.model->meshNo; aMesh++)
		{	
			mesh &thisMesh = aSprite.model->meshes[aMesh]; // referance variable

			/*	go through all the mesh's vertices and transform them by the 
				sprites final matrix, then store in the 'meshVertices' vector */
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//			ReplayManager::instance()->IntResult(thisMesh.virticesNo);
//			ReplayManager::instance()->MatrixResult(aSprite.finalMatrix);
//#endif
			for (int vertex = 0; vertex < thisMesh.virticesNo; vertex++)
			{
				CVector newVertex; // create new vertex
				meshVertices.push_back(newVertex); // store vertex in vector
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//				ReplayManager::instance()->VectorResult(thisMesh.vertices[vertex].originalXYZ);
//#endif

				// transform the mesh vertex and store in the vector
				meshVertices[vertex] =	aSprite.finalMatrix.multiplyVector(&
										thisMesh.vertices[vertex].originalXYZ);
//#ifdef DETERMINISTIC_REPLAY_DEBUG
//				ReplayManager::instance()->VectorResult(meshVertices[vertex]);
//#endif
			}
			
			// go through this mesh's triangles
#ifdef DETERMINISTIC_REPLAY_DEBUG
			ReplayManager::instance()->IntResult(thisMesh.trianglesNo);
#endif
			for (int triangle = 0; triangle < thisMesh.trianglesNo; triangle++)
			{	
				// store the triangle vertices required in an array
				CVector verts[] = {	CVector() = meshVertices[
									thisMesh.triangles[triangle].vert[0]], 
									CVector() = meshVertices[
									thisMesh.triangles[triangle].vert[1]],
									CVector() = meshVertices[
									thisMesh.triangles[triangle].vert[2]]};

				aTriangle tri;// create an atriangle instance
				tri.assocaitedSprite = &aSprite;// save relevant information to it

				// see if ellipsoid sprite and moving environment have hit
				triangleTest(sprite, boundingBoxCentre, boundingBoxHalfWidths,  
								spriteOldPosition, velocity, perCentOfMoveComplete, 
								polyintersectionPoint, collidedWith, previousHitPlane,
								repititions, velocDotVeloc, velocMagnitude, 
								&verts[0], &tri, &aPlane);
			}// end for triangle
			meshVertices.clear(); // clear the vector
		}// end for aMesh
	}// end for i

	previousHitPlane = aPlane; // set the previous hit plane
}// end findClosestTriangleMethod

// perform test against a single triangle
void CSpriteEngine::triangleTest(CSprite * sprite, CVector*, 
								CVector*,  
								CVector * spriteOldPosition, CVector * velocity,
								GLfloat & perCentOfMoveComplete, 
								CVector & polyintersectionPoint, 
								CSprite **collidedWith, CPlane &previousHitPlane,
								int repititions, float	velocDotVeloc, float velocMagnitude, 
								CVector * verts, aTriangle *tri, CPlane *aPlane)
{
#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(verts[0]);
	ReplayManager::instance()->VectorResult(verts[1]);
	ReplayManager::instance()->VectorResult(verts[2]);

	ReplayManager::instance()->FloatResult(perCentOfMoveComplete);
	ReplayManager::instance()->VectorResult(sprite->scaledElipsoidRadiusVector);
#endif

	// convert vertices to elipsoid space
	verts[0] /= &sprite->scaledElipsoidRadiusVector;
	verts[1] /= &sprite->scaledElipsoidRadiusVector;
	verts[2] /= &sprite->scaledElipsoidRadiusVector;
	// referance variable for speed and readability
	CVector &FirstVert = verts[0];

	/*	get the normal of the triangles plane 
		(can't divide triangle pane normal by elipsoid radius vector, won't work) */
	CVector planeNormal; 
	planeNormal.crossProduct(&verts[0], &verts[1], &verts[2]); 
#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(verts[0]);
	ReplayManager::instance()->VectorResult(verts[1]);
	ReplayManager::instance()->VectorResult(verts[2]);
#endif
	planeNormal.normalise();
						
	CPlane thisTriPlane(planeNormal, FirstVert); // create plane
	if (thisTriPlane == previousHitPlane){ // if this is the previous plane struck
		return;} // don't check it again, go to next triangle
#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(planeNormal);
	ReplayManager::instance()->VectorResult(*spriteOldPosition);	
#endif
	// get plane constant
	float	originDistance = thisTriPlane.planeConstant,
			// get spheres distance from plane
			distanceFromPlane = 
				( planeNormal.dotProduct(spriteOldPosition) ) + 
													originDistance,
			velocDotNorm = (planeNormal.dotProduct(velocity)),
			/*	variables for recording when the sphere makes contact with
				the triangle plane */
			t0, t1;

#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->FloatResult(originDistance);
	ReplayManager::instance()->FloatResult(distanceFromPlane);
#endif

	// if the starting point is behind the plane 
	if (distanceFromPlane < 0.0f){
		return;} // disregard starting point behind the plane

	// if sphere is running parallel with the triangle plane
	if (0.000001f > fabs(velocDotNorm))
	{
		// if sphere is not intersecting the triangle plane
		if (distanceFromPlane >= 0.9999f){
			return;} // go to next triangle
		// sphere is always in contact with the triangle plane
		t0 = 0.0f;
		//t1 = 1.0f;
	}
	else // sphere is not running parallel with triangle plane
	{
		// get time sphere makes contact with front of triangle plane
		t0 = (1.0f - distanceFromPlane) / velocDotNorm;
#ifdef DETERMINISTIC_REPLAY_DEBUG
		ReplayManager::instance()->FloatResult(t0);
#endif
		// get time sphere makes contact with back of triangle plane
		t1 = (-1.0f - distanceFromPlane) / velocDotNorm;
		// if heading away from the plane
		if (t1 < t0){
			return;} // no collision
	}
						
	// if the sphere hits the front of the triangle outside the velocity vector
	if (t0 > 1.0f){
		return;} // no collision

	/*	if the sphere did not begin in a position intersecting the 
		triangle plane, see if it intersected with the centre of the 
		triangle before checking the virtices and edges */
	if (distanceFromPlane >= 0.9f)
	{	// get the intersection point
		CVector intersectionPoint = (*spriteOldPosition - planeNormal) + 
									(*velocity * t0);

		// if the point is inside the triangle
		if (insideTriangle(&intersectionPoint, &FirstVert))
		{
			// if it is closer than the current closest triangle hit
			if (t0 <= perCentOfMoveComplete)
			{	
				if (repititions < 100)	
				{						
					// set closest triangle distance
					perCentOfMoveComplete = t0;
#ifdef DETERMINISTIC_REPLAY_DEBUG
					ReplayManager::instance()->FloatResult(perCentOfMoveComplete);
#endif
					// record triangle intersection point
					polyintersectionPoint = intersectionPoint;
					// record the sprite collidedWith
					*collidedWith = tri->assocaitedSprite;
					*aPlane = thisTriPlane; // record the plane struck
				}					
			}// end if closest triangle
			return; // collision found, go to next triangle
		}// end if points inside triangle
	}// end if originally not intersecting plane

	// check if sphere intersects the virtices of the triangle
	for (int virtex = 0; virtex < 3; virtex++)
	{
		// work out elements for quadratic equation
		float	a = velocDotVeloc, 
				b = 2.0f*(velocity->dotProduct((*spriteOldPosition - 
											verts[virtex]))),
				vertMinusOldPosMagnitude = 
					(verts[virtex] - spriteOldPosition).getMagnitude(),
				c = ( vertMinusOldPosMagnitude * vertMinusOldPosMagnitude )
					- 1.0f,
				lowestRoot;
		// see if sphere hits the virtex within velocity range
		if (solveQuadraticEquation(a, b, c, -0.1f, 1.0f, &lowestRoot))
		{
			// if it is closer than the current closest triangle hit
			if (lowestRoot <= perCentOfMoveComplete)
			{	
				CVector norm = 
					*spriteOldPosition + (*velocity * lowestRoot);
				norm -= &verts[virtex];
				norm.normalise(); // create normal
				CPlane plane(norm, verts[virtex]); // create plane
				// if its not the previous hit plane and repititions are less than 15
				if( (plane != previousHitPlane)
					&& (repititions < 15))
				{	
					*aPlane = plane; // record plane struck
					// set closest triangle distance
					perCentOfMoveComplete = lowestRoot;
					// record triangle intersection point
					polyintersectionPoint = verts[virtex];
					// record the sprite collidedWith
					*collidedWith = tri->assocaitedSprite;
				}
			}// end if closest triangle
		}
	}// end for virtices

	// check if the sphere intersects the edges of the triangle
	for (int perimeter = 0; perimeter < 3; perimeter++)
	{
		// work out elements for quadratic equation
		CVector	edge = verts[(perimeter + 1) % 3] - &verts[perimeter], 
				baseToVirtex = verts[perimeter] - spriteOldPosition; 

		float	edgeMagnitude			= edge.getMagnitude(),
				edgeDotVeloc			= edge.dotProduct(velocity), 
				baseToVertexMagnitude	= baseToVirtex.getMagnitude(),
				edgeDotBaseToVirtex		= edge.dotProduct(&baseToVirtex),
						
				a = (edgeMagnitude*edgeMagnitude) * 
					-(velocMagnitude * velocMagnitude) +
					(edgeDotVeloc * edgeDotVeloc),

				b = (edgeMagnitude * edgeMagnitude) * 
					(2.0f*(velocity->dotProduct(&baseToVirtex))) - 
					(2.0f*((edgeDotVeloc)*(edgeDotBaseToVirtex))),

				c = (edgeMagnitude * edgeMagnitude) * 
					(1.0f - (baseToVertexMagnitude * baseToVertexMagnitude)) +
					((edgeDotBaseToVirtex)*(edgeDotBaseToVirtex)),
				lowestRoot;
		/*	see if sphere hits the infinte line segment within velocity 
			range */
		if (solveQuadraticEquation(a, b, c, -0.1f, 1.0f, &lowestRoot))
		{
			/*	see if the intersection takes place on the section of the 
				infinite line which is this triangle perimeter edge */
			float edgeIntersect = 
				( (edgeDotVeloc)*lowestRoot - 
				(edgeDotBaseToVirtex) ) /
				(edgeMagnitude*edgeMagnitude);
			// see if hit within edge section
			if (edgeIntersect >= 0.0f && edgeIntersect <= 1.0f)
			{
				// if it is closer than the current closest triangle hit
				if (lowestRoot <= perCentOfMoveComplete)
				{	
					CVector norm = 
						*spriteOldPosition + (*velocity * lowestRoot), 
						edgeMultipliedByEdgeIntersect = (edge * edgeIntersect);
					norm -= &(edgeMultipliedByEdgeIntersect);
					norm.normalise(); // create normal
					// create plane
					CVector tempParamForPlane = (verts[perimeter] + edgeMultipliedByEdgeIntersect);
					CPlane plane(norm, tempParamForPlane);
					// if its not the previous hit plane and repititions are less than 15
					if( (plane != previousHitPlane) &&
						(repititions < 15))
					{
						*aPlane = plane; // record plane struck
						// set closest triangle distance
						perCentOfMoveComplete = lowestRoot;
						// record triangle intersection point
						polyintersectionPoint = 
							verts[perimeter] + &(edgeMultipliedByEdgeIntersect);
						// record the sprite collidedWith
						*collidedWith = tri->assocaitedSprite;
					}
				}// end if closest triangle
			}// end if edgeIntersect
		}// end if solveQuadraticEquation
	}// end for edges			
}

// draw all the sprites
void CSpriteEngine::drawAll(int timeChange, bool drawTransparentSprites) 
{
	int totalMovingSprites = (int)movingSprites.size();

	for (int sprite = 0; sprite < totalMovingSprites; sprite++)
	{	// referance variable for speed and readability
		CSprite &aSprite = *movingSprites[sprite];

		if (drawTransparentSprites == aSprite.transparency)
		{
			aSprite.draw(timeChange);
		}
	}
}

