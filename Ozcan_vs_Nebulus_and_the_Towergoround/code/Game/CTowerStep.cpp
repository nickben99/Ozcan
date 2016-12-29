//CTowerStep.cpp, a tower step class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CTowerStep.h"
//-------------------------

//constructor
CTowerStep::CTowerStep()
{
}

// overloaded constructor
CTowerStep::CTowerStep(CModel * aModel)
	: CTowerSprite(aModel) 
{
}

// overloaded constructor
CTowerStep::CTowerStep(CModel * aModel, int towerPosition)
	: CTowerSprite(aModel, towerPosition) // initialise super class
{
	initialise(towerPosition); // initialise the class
}

// class initialisation
void CTowerStep::initialise(int)
{
	// create a vector to move the step out to the side of the tower
	CVector tempZTran(	0.0f, 
						0.0f,
						scale.z*(BASE_MEASUREMENT_Z*0.5f));
	
	// rotate by the steps rotation
	tempZTran = rotation.multiplyRotateVector(&tempZTran);
	// add the rotated vector onto the position vector of the step
	position += &tempZTran;
	collisionType = TRIANGLES; /*	every triangle of the associated model will be checked
									during collision detection */
	spriteType = SSTEP; // set sprite type
	dynamicProperty = STATIC_ENVIRONMENT; // a non moving section of the environment

	oldPosition = position; /*	set these here and call calculateFinalMatrix so they do 
								not need to be called every frame if the sprite does not 
								move and ensures its in the correct position after 
								reinitialising */
	calculateFinalMatrix();
}

// reset the sprite to starting state
void CTowerStep::reinitialise()
{	
	CTowerSprite::reinitialise();
	CTowerStep::initialise(startingTowerPosition); // call this class' initialise
}

// destructor
CTowerStep::~CTowerStep()
{
}
