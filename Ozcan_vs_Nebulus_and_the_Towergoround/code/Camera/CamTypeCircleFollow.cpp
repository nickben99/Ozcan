//CCamera.cpp, the camera class

// system includes --------
#include <math.h>
#include <assert.h>
//-------------------------

// header files -----------
#include "CamTypeCircleFollow.h"
#include "CameraTransform.h"

//-------------------------

// constructor
CamTypeCircleFollow::CamTypeCircleFollow()
{
	theNebulus = 0; // initially set to 0
	initialise();
}

// do any initialisation
void CamTypeCircleFollow::initialise()
{
	// initialise FAR_OUT camera view variables
	FAR_OUTnebulusOffset = 0.0f;
	FAR_OUTcameraPositionToSceneCentreRestLength = 15.0f*1.5f;
	FAR_OUTrelativeCameraHeight = (6.0f*1.5f)+6.0f;

	/*	various springConstant, spring inner friction and spring 
		resting length values */
	sceneCentreToNebulusSpringConstant = -30.0f; // springConstants should be negative
	sceneCentreToNebulusFrictionConstant = 4.0f;
	sceneCentreToNebulusRestLength = 0.0f; 
	cameraPositionYSpringConstant = -10.0f; // springConstants should be negative
	cameraPositionYFrictionConstant = 4.0f;
	cameraPositionYRestLength = 0.0f;
	cameraPositionToSceneCentreSpringConstant = -30.0f; // springConstants should be negative
	cameraPositionToSceneCentreFrictionConstant = 4.0f;
	cameraPositionToSceneCentreRestLength = FAR_OUTcameraPositionToSceneCentreRestLength;

	// how far above the scene centre the camera position is
	relativeCameraHeight = FAR_OUTrelativeCameraHeight;

	/*	set the nebulus offset (nebulus offset is required as when the camera is in 
		close up mode it needs to be a bit above nebulus.  this cannot be achieved by 
		cameraPositionYRestLength as this can result in the camera ending up the 
		specified length above or below nebulus, whereas it need to be bove) */
	nebulusOffset = FAR_OUTnebulusOffset;

	// get Nebulus instance
	theNebulus = CNebulus::getExistingInstance();

	if (0 != theNebulus)
	{
		// initialise the starting camera position
		cameraPosition.initialise();
		cameraPosition.position = theNebulus->position;
		cameraPosition.position.y = 0.0f;
		cameraPosition.position.setMagnitude(cameraPosition.position.getMagnitude()+cameraPositionToSceneCentreRestLength);

		cameraPositionY.initialise();
		cameraPositionY.position.y = theNebulus->position.y;

		// initialise the centre of the scene position
		sceneCentre.initialise();
		sceneCentre.position = theNebulus->position;

		// reset all the forces and velocities when the camera is reinitialised
		sceneCentre.forces.reset();
		sceneCentre.velocity.reset();
		cameraPositionY.forces.reset();
		cameraPositionY.velocity.reset();
		cameraPosition.forces.reset();
		cameraPosition.velocity.reset();

		SetSpritePosition();
	}
}

// dolly the camera into position
void CamTypeCircleFollow::move(GLfloat perCentOfSecond)
{
	/*	this section of code works out the spring from the spot the camera is looking at 
		to nebulus, on the ground plane (y is not considered) */
	CVector sceneCentreXZ	= sceneCentre.position,
			nebulusXZ		= theNebulus->position; // get sceneCentre and Nebulus position
	// scene centre only follows nebulus on the ground plane, so set y to zero
	sceneCentreXZ.y = nebulusXZ.y = 0.0f;  

	/*	find a vector from the position of nebulusXZ to sceneCentreXZ 
		(this seems wrong way around, but springvector is negative to account for this) */
	CVector springVector	= sceneCentreXZ - &nebulusXZ; 
	float	r				= springVector.getMagnitude(), // get the magnitude
			springLength	= sceneCentreToNebulusRestLength; // the length the spring should be at rest
		
	sceneCentre.forces.reset(); // reset the forces before adding them on below
	// work out the force which attracts sceneCentreXZ to nebulusXZ
	CVector force = ((springVector / r) * ((r - springLength) * (sceneCentreToNebulusSpringConstant)));
	sceneCentre.applyForce( &force );	//the spring force is added

	// get velocity and set y to 0 as this is not considered
	CVector sceneCentreVelocXZ = sceneCentre.velocity;
	sceneCentreVelocXZ.y = 0.0f;

	// apply friction force with veloc negated as it is acting in the opposite direction
	sceneCentre.applyForce((-(sceneCentreVelocXZ) * sceneCentreToNebulusFrictionConstant));

	// move the scene centre
	sceneCentre.move(0, perCentOfSecond);

	/*	make a copy of sceneCentre position with y set to 0, this will be used to set the camera
		(further down) */
	sceneCentreXZ =  sceneCentre.position;
	sceneCentreXZ.y = 0.0f;

	/*	this section of code works out the spring from the cameraPositionY to nebulus' y position, 
		only the y value are considered here */
	CVector nebulusY;
	// get nebulus y (stop camera going further down than -5.0f)
	nebulusY.y = (theNebulus->position.y > -5.0f) ? theNebulus->position.y+nebulusOffset : -5.0f;
	//nebulusY.y = theNebulus->position.y+nebulusOffset;

	// find a vector from nebulusY to cameraPositionY
	springVector	= cameraPositionY.position - &nebulusY; 
	r				= springVector.getMagnitude(); // get the magnitude of the spring vector
	springLength	= cameraPositionYRestLength;	// set the length of the spring at rest
		
	cameraPositionY.forces.reset(); // reset the forces in preperation for adding below
	// the spring force is added
	cameraPositionY.applyForce(((springVector / r) * ((r - springLength) * (cameraPositionYSpringConstant))));	
	// apply friction force with veloc negated as it is acting in the opposite direction
	cameraPositionY.applyForce((-(cameraPositionY.velocity) * cameraPositionYFrictionConstant));
	// move cameraPositionY
	cameraPositionY.move(0, perCentOfSecond);

	/*	this section of code works ou the ground plane spring (x, z no y) from the camera 
		position to the spot the camera is looking at */
	CVector cameraPositionXZ = cameraPosition.position; // get camera position
	cameraPositionXZ.y = 0.0f; // set y to zero as only working on x, z axis
	// find a vector from sceneCentreXZ to cameraPositionXZ
	springVector	= cameraPositionXZ - &sceneCentreXZ; 
	r				=	springVector.getMagnitude(); // get magnitude of springVector
	springLength	=	cameraPositionToSceneCentreRestLength; // the resting length of the spring
		
	cameraPosition.forces.reset(); //reset the forces before adding below
	//the spring force is added
	cameraPosition.applyForce( ((springVector / r) * ((r - springLength) * (cameraPositionToSceneCentreSpringConstant))));
	
	// get cameraPosition velocity on the ground plane (set y to zero)
	CVector cameraPositionVelocXZ = cameraPosition.velocity;
	cameraPositionVelocXZ.y = 0.0f;
	// apply friction force with veloc negated as it is acting in the opposite direction
	cameraPosition.applyForce((-(cameraPositionVelocXZ) * cameraPositionToSceneCentreFrictionConstant));

	// move the cameraPosition
	cameraPosition.move(0, perCentOfSecond);
	// make copy of cameraPosition with no y element
	cameraPositionXZ = cameraPosition.position;
	cameraPositionXZ.y = 0.0f;

//------------------------------------------------------------

	// get vector from sceneCentre to cameraPosition
	CVector relative =	cameraPosition.position - &sceneCentre.position;

	// set amount to move this frame
	float	movementThisFrame = perCentOfSecond*(PI);

	/*  this section of code ensures the camera keeps following Nebulus around the 
		tower */
	
	//if(circle)
	{
		// get nebulus x, z (set y to zero) 
		CVector nebulusPositionXZ = sceneCentreXZ;
		nebulusPositionXZ.y = 0.0f;
		
		/* get angle between positions (cameraPositions y value should always be zero, but 
										should probably use cameraPositionXZ) */
		float angle = cameraPosition.position.getAngleBetween(&nebulusPositionXZ);
		// dont go over the angle between
		if (movementThisFrame > angle){
			movementThisFrame = angle;}
		// get the cross product of the positions and normalise
		CVector cross;
		cross.crossProduct(&cameraPosition.position, &nebulusPositionXZ);
		cross.normalise();
		/*	if the cross product could not be derived, which could happen if the vectors are 
			at an angle of 180 degrees from one and other and thus parallel and no plane can 
			be derived (also potentially if vectors are in the same direction) */
		if (cross.fuzzyEquals(CVector())){
			cross = CVector(0.0f, 1.0f, 0.0f);} // set axis
		// create the matrix for the movement this frame
		CMatrix move3;
		move3.createArbitraryAxisRotation(movementThisFrame, cross);
		// rotate the relative vector and the camera position
		relative = move3.multiplyRotateVector(&relative);
		cameraPosition.velocity = 
			move3.multiplyRotateVector(&cameraPosition.velocity);

		// add the relative vector onto scene centre to get new cameraPosition
		cameraPosition.position = sceneCentre.position + &relative;
	}
//-------------------------------------------------------------
	SetSpritePosition();
}

void CamTypeCircleFollow::SetSpritePosition()
{
	// get the position of the camera 
	position.position = CVector(	cameraPosition.position.x, 
						/*	always the same distance above the scene centre spot, the cameraPositionY
							follows nebulus' y so nebulus goes up and down on screen, means the camera 
							does not go rigidly up and down with nebulus, this seems to be how it is done
							with Mario */
						cameraPositionY.position.y+relativeCameraHeight, 
						cameraPosition.position.z);
}

CVector CamTypeCircleFollow::GetLookAt()
{
	return CVector(sceneCentre.position.x, cameraPositionY.position.y, sceneCentre.position.z);
}

void CamTypeCircleFollow::GetOutput(CameraTransform& transform)
{
	CVector lookAt = GetLookAt();
	transform.SetLookAt(lookAt);

	CVector forward = lookAt-GetPosition();
	forward.normalise(); // shouldn't need to normalise here, but incorrect quaternion gets created if I don't
	transform.SetOrientation(CMatrix(forward).matrixToQuaternion());
	transform.SetDistanceFromLookAt((lookAt-GetPosition()).getMagnitude());

//#ifdef _DEBUG
//	static bool doRepeat = true;
//	while (doRepeat)
//	{
//		doRepeat = false;
//
//		CVector lookAt = GetLookAt();
//		CVector forward = lookAt-GetPosition();
//		forward.normalise();
//		//float length = forward.getMagnitude();
//		//UNUSED(length);
//		CMatrix newMat(forward);
//		CQuaternion quat = newMat.matrixToQuaternion();
//
//		CMatrix fromQuat(quat);
//		CVector forwardCpy = fromQuat.forward();
//		if (!CVector::SafeNormalizeXZ(forwardCpy))
//		{
//			assert(false);
//			//doRepeat = true;
//		}
//
//		CVector lookAt11 = GetLookAt();
//		CVector forward11 = lookAt11-GetPosition();
//		CMatrix newMat11(forward11);
//		CQuaternion quat11 = newMat11.matrixToQuaternion();
//
//		CMatrix fromQuat11(quat11);
//		CVector forwardCpy11 = fromQuat11.forward();
//		if (!CVector::SafeNormalizeXZ(forwardCpy11))
//		{
//			assert(false);
//			//doRepeat = true;
//		}		
//	}
//#endif
}