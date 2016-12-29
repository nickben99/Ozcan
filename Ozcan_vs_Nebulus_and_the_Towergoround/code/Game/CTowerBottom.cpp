//CTowerBottom.cpp, the tower bottom class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CTowerBottom.h"
//-------------------------

// constructor
CTowerBottom::CTowerBottom(CModel * aModel)
	: CTowerSprite(aModel) // initialise super class
{
	initialise();
}

// initialise class
void CTowerBottom::initialise()
{
	// set the scale of the model
	scale.x = scale.y = scale.z = theTower->towerWallPos*2.0f;
	scale.y *= 2.0f;
	// set current bounding box and elipsoid radius vectors based on scale
	scaledBoundingBox = boundingBox * &scale;
	scaledHalfBoundingBox = scaledBoundingBox * 0.5f;
	scaledElipsoidRadiusVector = elipsoidRadiusVector * &scale;
	
	// model goes in the centre of the tower
	position.x = position.z = 0.0f;
	// model goes at the bottom of the tower
	position.y -= ( (scale.y*0.5f) + (theTower->towerRowHeight*0.5f) );

	collisionType = TRIANGLES; /*	every triangle of the associated model will be checked
									during collision detection */
	spriteType = STOWER_TOP_OR_BASE; // set sprite type
	dynamicProperty = STATIC_ENVIRONMENT; // a non moving section of the environment

	oldPosition = position; /*	set these here and call calculateFinalMatrix so they do 
								not need to be called every frame if the sprite does not 
								move */
	calculateFinalMatrix();
}

// destructor
CTowerBottom::~CTowerBottom()
{
}
