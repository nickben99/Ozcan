// CamTypeReplay.cpp, the camera class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CamTypeReplay.h"

#include "Game/CNebulus.h"
#include "Game/CLevel.h"
#include "CameraTransform.h"
#include "Game/Globals.h"
//-------------------------

//--- external variables --------
//-------------------------------

// constructor
CamTypeReplay::CamTypeReplay()
{
	initialise();
}

// do any initialisation
void CamTypeReplay::initialise()
{
	if (0 == CNebulus::getExistingInstance())
	{
		return;
	}

	CVector offset = CNebulus::getExistingInstance()->position;
	offset.y = 0.0f;
	offset.normalise() *= 6.0f;
	offset.y = 2.0f;

	position = CNebulus::getExistingInstance()->position + offset;

	// this section pushes the camera to one side to follow nebulus
	CVector sideVector(position.z, 0.0f, -position.x);
	sideVector.normalise() *= 4.0f;

	if (sideVector.dotProduct(CNebulus::getExistingInstance()->rotation.forward()) > 0.0f)
	{
		sideVector = -sideVector;
	}
	position += sideVector;
	move(0.0f);
}

// dolly the camera into position
void CamTypeReplay::move(GLfloat)
{
	CVector nebulousPos = CNebulus::getExistingInstance()->position;
	CVector previousCameraPos = position;

	CVector offset = previousCameraPos - nebulousPos;
	offset.y = 0.0f;
	offset.normalise() *= 6.0f;

	float stepEndDist = Globals::Instance().currentLevel.theScene.tower->towerWallPos + (BASE_MEASUREMENT_Z*Globals::Instance().currentLevel.theScene.tower->xPercentFromBaseMeasurement);
	position = CNebulus::getExistingInstance()->position + offset;
	if (position.getMagnitudeSquared() < stepEndDist*stepEndDist) // don't let camera get closer to the tower than the edge of the step
	{
		float y = position.y;
		position.y = 0.0f;
		position.normalise() *= stepEndDist;
		position.y = y;
	}

	position.y += 2.0f;
}

CVector CamTypeReplay::GetLookAt()
{
	return CNebulus::getExistingInstance()->position;
}

void CamTypeReplay::GetOutput(CameraTransform& transform)
{
	CVector lookAt = GetLookAt();
	transform.SetLookAt(lookAt);

	CVector forward = lookAt-GetPosition();
	transform.SetOrientation(CMatrix(forward).matrixToQuaternion());
	transform.SetDistanceFromLookAt(forward.getMagnitude());
}