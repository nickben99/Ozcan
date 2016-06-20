// CamTypeTunnel.cpp, the camera class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CamTypeTunnel.h"

#include "Game/CNebulus.h"
#include "Game/CLevel.h"
#include "CameraTransform.h"
#include "Camera/CameraTransitionCollision.h"
#include "Game/Globals.h"
//-------------------------

static float kGroundPlaneOffset = 3.0f;
static float kVirticalOffset = 0.7f;
static float kVirticalLookAtOffset = 0.5f; // 1.2f

//--- external variables --------
//-------------------------------

// constructor
CamTypeTunnel::CamTypeTunnel()
{
	initialise();

#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->VectorResult(lookAt);
	ReplayManager::instance()->VectorResult(offset);
	//ReplayManager::instance()->FloatResult(orientation);
	ReplayManager::instance()->FloatResult(orientationSpeed);
	ReplayManager::instance()->VectorResult(position);
#endif
}

// do any initialisation
void CamTypeTunnel::initialise()
{
	if (0 == CNebulus::getExistingInstance())
	{
		return;
	}

	mpTunnel = CNebulus::getExistingInstance()->GetTunnel();

	if (0 == mpTunnel)
	{
		return;
	}

	CVector nebulousPos = CNebulus::getExistingInstance()->position;
	lookAt = nebulousPos + CVector(0.0f, kVirticalLookAtOffset, 0.0f);
	CMatrix tunnelRotation = mpTunnel->rotation;
	CVector tunnelPosition = mpTunnel->position;
	CVector tunnelScaledHalfBoundingBox = mpTunnel->scaledHalfBoundingBox;

	tunnelPosition.setY(0.0f);
	nebulousPos.setY(0.0f);
	
	// project camera onto the forward vector of the tunnel, based on nebulus position
	float projection = (nebulousPos - tunnelPosition).dotProduct(tunnelRotation.forward());
	// calculate offset of camera to nebulus
	offset = (tunnelRotation.forward().v3 * (projection / fabs(projection))) * kGroundPlaneOffset;

	//orientation = 0.0f;

	position = lookAt + offset; //lookAt + CVector(cos(orientation), 0.0f, sin(orientation)) * kGroundPlaneOffset; //offset;
	position.y += kVirticalOffset;
}

// dolly the camera into position
void CamTypeTunnel::move(GLfloat /*delta*/)
{
	CVector nebulousPos = CNebulus::getExistingInstance()->position;
	lookAt = nebulousPos + CVector(0.0f, kVirticalLookAtOffset, 0.0f);

	position = lookAt + offset;
	position.y += kVirticalOffset;

	CVector hitPoint;
	CVector hitNormal;
	if (Globals::Instance().currentLevel.theScene.SphereCast(lookAt, position, CameraTransitionCollision::kCollisionRadius, true, false, hitPoint, hitNormal))
	{
		position = hitPoint;
	}
}

void CamTypeTunnel::GetOutput(CameraTransform& transform)
{
	transform.SetLookAt(lookAt);

	CVector forward = lookAt-GetPosition();
	transform.SetOrientation(CMatrix(forward).matrixToQuaternion());
	transform.SetDistanceFromLookAt(forward.getMagnitude());
}