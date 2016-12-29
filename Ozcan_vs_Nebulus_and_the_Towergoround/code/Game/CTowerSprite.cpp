//CTowerSprite.cpp, a tower sprite class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CTowerSprite.h"

//-------------------------

// constructor
CTowerSprite::CTowerSprite()
{
}

// overloaded constructor
CTowerSprite::CTowerSprite(CModel * aModel)
	: CSprite(aModel) 
{
	theTower = CTower::instance(); // get singleton tower instance
}

// overloaded constructor
CTowerSprite::CTowerSprite(CModel * aModel, int towerPosition)
	: CSprite(aModel) // initialise super class
{
	theTower = CTower::instance(); // get singleton tower instance
	initialise(towerPosition); // initialise class
}

// initialise the class
void CTowerSprite::initialise(int towerPosition)
{
	/*	record the starting tower position in case the sprite 
		needs to be reinitialised in that place */
	startingTowerPosition = towerPosition;

	position.reset(); // reset position
	rotation.identity(); // reset rotation

	// work out starting row and column
	int startingRow		= towerPosition/theTower->towerColumns;
	int startingColumn	= towerPosition%theTower->towerColumns;

	position.setY((float)startingRow*theTower->towerRowHeight); // translate up y axis
	position.setZ(theTower->towerWallPos); // translate along z axis
	
	CMatrix tempYRot;
	// create rotation matrix for y rotation of column
	tempYRot.createYRotationMatrix(
		degToRad(theTower->angleBetweenTowerSegments)*startingColumn);
	
	// multiply position by y rotation matrix
	position = tempYRot.multiplyRotateVector(&position);
	rotation = tempYRot; // set starting rotation of sprite

	scale.x = theTower->xPercentFromBaseMeasurement; // set scaling factors of object
	scale.y = theTower->yPercentFromBaseMeasurement;
	/*	the z axis of the tower sprite is scaled according to the X AXIS SCALING, this 
		is because the z is sclaed in the same proportion to the x axis */
	scale.z = theTower->xPercentFromBaseMeasurement;
	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;
	dynamicProperty = STATIC_ENVIRONMENT; // a non moving section of the environment 

	oldPosition = position; /*	set these here and call calculateFinalMatrix so they do 
								not need to be called every frame if the sprite does not 
								move and ensures its in the correct position after 
								reinitialising */
	calculateFinalMatrix();
}

// reinitialise sprite to starting state
void CTowerSprite::reinitialise( void)
{
	CSprite::reinitialise(); // call CSprite reinitialise first (same order as when constructed)
	CTowerSprite::initialise(startingTowerPosition); // call this class' initialise method
}

// draw the sprite at appropriate frame
void CTowerSprite::draw(int timeChange)
{
	CSprite::draw(timeChange);
}

// destructor
CTowerSprite::~CTowerSprite()
{
}





