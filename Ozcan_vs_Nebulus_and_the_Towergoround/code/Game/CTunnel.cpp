//CTunnel.cpp, a tower tunnel class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CTunnel.h"
//-------------------------

// constructor
CTunnel::CTunnel(CModel * aModel, int towerPosition)
	: CTowerSprite(aModel, towerPosition) // initialise super class
{
	initialise();
}

// initialise the class
void CTunnel::initialise()
{
	// create a vector to move the step out to the side of the tower
	CVector tempZTran(	0.0f, 
						0.0f,
						-theTower->towerWallPos);
	
	// rotate by the tunnels rotation
	tempZTran = rotation.multiplyRotateVector(&tempZTran);
	// add the rotated vector onto the position vector of the tunnel
	position += &tempZTran;
	// scale tunnel so it compleatly fills the inside of the tower
	scale.z = ((theTower->towerWallPos)*2.0f);

	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;

	collisionType = TRIANGLES; /*	every triangle of the associated model will be checked
									during collision detection */
	spriteType = STUNNEL; // set sprite type
	dynamicProperty = STATIC_ENVIRONMENT; // a non moving section of the environment

	oldPosition = position; /*	set these here and call calculateFinalMatrix so they do 
								not need to be called every frame if the sprite does not 
								move */
	calculateFinalMatrix();
}

// destructor
CTunnel::~CTunnel()
{
	CTowerSprite::~CTowerSprite(); // call super class destructor
}
