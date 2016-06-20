//CCamera.cpp, the camera class

// system includes --------
#include <math.h>
#include <assert.h>
//-------------------------

// header files -----------
#include "CCamera.h"
#include "CameraTransform.h"
#include "Game/Globals.h"
#include "Camera\CameraLerpDistanceElevation.h"
//-------------------------

// creates singleton camera instance
CCamera* CCamera::instance() 
{
	static CCamera cameraInstance; // create static camera instance once
    return &cameraInstance; // return the address of the camera instance
}

// constructor
CCamera::CCamera()
{
	theNebulus = 0; // initially set to 0
}

// destructor
CCamera::~CCamera()
{
}

// do any initialisation
void CCamera::initialise()
{
	mActiveTypes.Initialize();
	mPriorityTypes.Initialize();

	mLinks.Reset();
	const unsigned int collisionDetectDynamicType = (1 << STATIC_ENVIRONMENT);
	const unsigned int collisionDetectSpriteTypes = (1 << STUNNEL);
	mLinks.AddLink(CamType::eCICircleFollow, CamType::eCITunnel, new CameraLerp(0.5f, collisionDetectDynamicType, collisionDetectSpriteTypes)); 
	mLinks.AddLink(CamType::eCITunnel, CamType::eCICircleFollow, new CameraLerpDistanceElevation(1.0f, collisionDetectDynamicType, collisionDetectSpriteTypes)); 
	const unsigned int kNoCollision = 0;
	mLinks.AddLink(CamType::eCIMax, CamType::eCIReplay, new CameraLerp(-1.0f, kNoCollision, kNoCollision)); // anything into replay will be a cut

	// initialise CLOSE_UP camera view variables
	CLOSE_UPnebulusOffset = 0.5f; // set to 1.0f when nebulus is big
	CLOSE_UPcameraPositionToSceneCentreRestLength = 6.0f; 
	CLOSE_UPrelativeCameraHeight = 1.0f;

	// initialise FAR_OUT camera view variables
	FAR_OUTnebulusOffset = 0.0f;
	FAR_OUTcameraPositionToSceneCentreRestLength = 15.0f*1.5f;
	FAR_OUTrelativeCameraHeight = (6.0f*1.5f)+6.0f;

	// how long the camera takes to change from one view to another (in seconds)
	cameraChangePeriod = 1.0f;

	cameraMode = FAR_OUT; // set camera mode
	Globals::Instance().gl.GLEnable(GL_CULL_FACE); // face culling required in far out mode	
	Globals::Instance().gl.GLDisable(GL_CULL_FACE);  ///////////////////////////////////////////////////////////////

	// the camera is initially not in a transition period
	changingCameraMode = 0;

	// get Nebulus instance
	theNebulus = CNebulus::getExistingInstance();

	mFollowCam.initialise();
	mStaticReplay.initialise();

	mPriorityTypes.Add(CameraPriorityTypes::CamTypeAndData(CamType::eCICircleFollow, 0));
	TryChangeCamera(false);
	SetCameraOutputVariables(false);
}

// set the camera mode
void CCamera::setCameraMode(cameraModes modeRequired)
{
	//modeRequired = TUNNEL_CAM;
	switch(modeRequired)
	{
		case TUNNEL_CAM:
		{
			//if (0 != theNebulus->GetTunnel()) // we can't change to tunnel cam until nebulus has logged that he's inside a tunnel
			{
				//mTunnelCam.initialise();
				cameraMode = modeRequired; // set camera mode
				Globals::Instance().gl.GLDisable(GL_CULL_FACE); // face culling not required in close up mode
				changingCameraMode = 0; // the camera is now in a changeover period
				cameraChangeTimer = 0.0f;
			}
			break;
		}
		case CLOSE_UP:
			// set the lerping variables
			nebulusOffsetLERP_LENGTH = CLOSE_UPnebulusOffset - mFollowCam.nebulusOffset;
			cameraPositionToSceneCentreRestLengthLERP_LENGTH = CLOSE_UPcameraPositionToSceneCentreRestLength - mFollowCam.cameraPositionToSceneCentreRestLength;
			relativeCameraHeightLERP_LENGTH = CLOSE_UPrelativeCameraHeight - mFollowCam.relativeCameraHeight;

			cameraMode = modeRequired; // set camera mode
			Globals::Instance().gl.GLDisable(GL_CULL_FACE); // face culling not required in close up mode
			changingCameraMode = 1; // the camera is now in a changeover period
			cameraChangeTimer = 0.0f;
			break;
		case FAR_OUT:
			mFollowCam.initialise();
			// set the lerping variables
			nebulusOffsetLERP_LENGTH = FAR_OUTnebulusOffset - mFollowCam.nebulusOffset;
			cameraPositionToSceneCentreRestLengthLERP_LENGTH = FAR_OUTcameraPositionToSceneCentreRestLength - mFollowCam.cameraPositionToSceneCentreRestLength;
			relativeCameraHeightLERP_LENGTH = FAR_OUTrelativeCameraHeight - mFollowCam.relativeCameraHeight;

			cameraMode = modeRequired; // set camera mode
			Globals::Instance().gl.GLEnable(GL_CULL_FACE); // face culling required in far out mode
			Globals::Instance().gl.GLDisable(GL_CULL_FACE);  ///////////////////////////////////////////////////////////////

			changingCameraMode = 0;//1; // the camera is now in a changeover period
			cameraChangeTimer = 0.0f;
			break;

		case STATIC_REPLAY:
			cameraMode = modeRequired; // set camera mode
			Globals::Instance().gl.GLEnable(GL_CULL_FACE); // face culling required in far out mode
			Globals::Instance().gl.GLDisable(GL_CULL_FACE);  ///////////////////////////////////////////////////////////////

			changingCameraMode = 0; // no change over period - instant
			cameraChangeTimer = 0.0f;
			break;
		
		default: break;
	}
}

CamType* Create(CamType::eCamIdentifier identifier)
{
	switch (identifier)
	{
	case CamType::eCICircleFollow:
		return new CamTypeCircleFollow();
	case CamType::eCIReplay: 
		return new CamTypeReplay();
	case CamType::eCITunnel: 
		return new CamTypeTunnel();
	case CamType::eCIMax:
	default: 
		assert(false);
		break;
	}
	return 0;
}

static void CalculateControls(const CVector& cameraForward, CVector& outForwardControl, CVector& outRightControl)
{
	outForwardControl = cameraForward;
	// work out camera forward and right vectors
	outForwardControl.y = 0.0f;
	outForwardControl.normalise();
	// get the right vector from the forward vector
	outRightControl.z = outForwardControl.x;
	outRightControl.x = -outForwardControl.z;
}

// dolly the camera into position
void CCamera::Update()
{
	if (GameTime::Instance().IsPaused())
	{
		return;
	}

#ifdef DETERMINISTIC_REPLAY_DEBUG
	CameraTransform trans;
	mActiveTypes.GetOutput(trans);
	trans.ReplayCheck();	
#endif

	TryChangeCamera(true);
	mActiveTypes.Update();

#ifdef DETERMINISTIC_REPLAY_DEBUG
	CameraTransform postTrans;
	mActiveTypes.GetOutput(postTrans);
	postTrans.ReplayCheck();		
#endif
}

void CCamera::TryChangeCamera(bool doReplayChecks)
{
#ifndef DETERMINISTIC_REPLAY_DEBUG
	UNUSED(doReplayChecks);
#endif

	CameraPriorityTypes::CamTypeAndData highestPriority;
	if (mPriorityTypes.GetHighestPriority(highestPriority) && highestPriority.mIdentifier != mActiveTypes.GetNewestCameraType())
	{
		CameraLerp* pLerp = mLinks.CreateNewLerpFromTemplate(mActiveTypes.GetNewestCameraType(), highestPriority.mIdentifier); 
		mActiveTypes.ChangeTo(Create(highestPriority.mIdentifier), pLerp);
	}

#ifdef DETERMINISTIC_REPLAY_DEBUG
	if (doReplayChecks)
	{
		ReplayManager::instance()->IntResult((int)highestPriority.mIdentifier);
	}
#endif
}

void CCamera::SetCameraOutputVariables(bool doReplayChecks)
{
#ifndef DETERMINISTIC_REPLAY_DEBUG
	UNUSED(doReplayChecks);
#endif

	CameraTransform trans;
	mActiveTypes.GetOutput(trans);

#ifdef DETERMINISTIC_REPLAY_DEBUG
	if (doReplayChecks && !GameTime::Instance().IsPaused())
	{
		trans.ReplayCheck();
	}
#endif

	position = trans.CalculatePosition();
	lookAt = trans.GetLookAt();

#ifdef DETERMINISTIC_REPLAY_DEBUG
	if (doReplayChecks && !GameTime::Instance().IsPaused())
	{
		ReplayManager::instance()->VectorResult(lookAt);
		ReplayManager::instance()->VectorResult(position);		
	}
#endif

	// nebulus requires a forward and right vector in order to move correctly depending on the direction the camera is pointing
	CalculateControls(lookAt-position, forwardControl, rightControl);

#ifdef DETERMINISTIC_REPLAY_DEBUG
	if (doReplayChecks && !GameTime::Instance().IsPaused())
	{
		ReplayManager::instance()->VectorResult(forwardControl);
		ReplayManager::instance()->VectorResult(rightControl);
	}
#endif

	// this section of code works out values which are used by the sky board to ensure the scenary is rotated correctly -----------
	float anAng = CVector::unitZ.getAngleBetween(&forwardControl);
	
	// get cross product of the two vectors and normalise
	CVector aCross;
	aCross.crossProduct(CVector::unitZ, forwardControl);
	aCross.normalise();
	/*	if the cross product could not be derived, which could happen if the vectors are 
		at an angle of 180 degrees from one and other and thus parallel and no plane can 
		be derived (also potentially if vectors are in the same direction) */
	if (aCross.fuzzyEquals(CVector())){
		aCross = CVector(0.0f, 1.0f, 0.0f);} // set axis

	if (aCross.fuzzyEquals(CVector(0.0f, -1.0f, 0.0f)))
	{
		aCross = CVector(0.0f, 1.0f, 0.0f);
		anAng = (2.0f*PI)-anAng;
	} // set axis
	currentYRotation = anAng; // currentYRotation used by sky board

	// create rotation matrix from angle and axis
	rotation.createYRotationMatrix(anAng); // rotation used by sky board
// --------------------------------------
}

void CCamera::SetCamera()
{
	CVector finalPosition;
	CVector finalLookAt;
	CVector finalUpVector(0.0f, 1.0f, 0.0f);

	//modelViewMatrix = CMatrix::LookAt(position, lookAt, finalUpVector);

#if _DEBUG
	mDebugCam.Update();
	if (!mDebugCam.IsActive())
#endif
	{
		finalPosition = position;
		finalLookAt = lookAt;
	}
#if _DEBUG
	else
	{
		CQuaternion debugOrientation;
		mDebugCam.GetOutput(finalPosition, debugOrientation);
		CMatrix debugCameraOrientationMatrix(debugOrientation);
		finalLookAt = finalPosition + debugCameraOrientationMatrix.forward();
		finalUpVector = debugCameraOrientationMatrix.up();
	}
#endif

	modelViewMatrix = CMatrix::LookAt(finalPosition, finalLookAt, finalUpVector);
	//CMatrix mat = CMatrix::LookAt(finalPosition, finalLookAt, finalUpVector);
	Globals::Instance().viewMatrixStack.MultMatrix(modelViewMatrix);

//	gluLookAt(finalPosition.x, finalPosition.y, finalPosition.z, // viewing position			  
//			finalLookAt.x, finalLookAt.y, finalLookAt.z, // centre of scene being viewed
//			finalUpVector.x, finalUpVector.y, finalUpVector.z); // up vector
}

// debug or game camera
CVector CCamera::GetActiveCameraPosition()
{
#if _DEBUG
	if (IsDebugCamActive())
	{
		return GetDebugCameraPosition();
	}
#endif
	return GetGameCameraPosition();
}

CVector CCamera::GetGameCameraPosition()
{
	return position;
}

CVector CCamera::GetGameCameraControlForward()
{
	return forwardControl;
}

CVector CCamera::GetGameCameraControlRight()
{
	return rightControl;
}

CQuaternion CCamera::GetGameCameraOrientation()
{
	CMatrix transform(lookAt-position);
	return transform.matrixToQuaternion();
}

CMatrix CCamera::GetGameCameraSkyBoardRotation()
{
	return rotation;
}

#if _DEBUG
bool CCamera::IsDebugCamActive()
{
	return mDebugCam.IsActive();
}

bool CCamera::IsDebugCamPositionLocked()
{
	return mDebugCam.IsPositionLocked();
}

CVector CCamera::GetDebugCameraPosition()
{
	return mDebugCam.GetPosition();
}

CVector CCamera::GetDebugCameraControlForward()
{
	CMatrix transform = mDebugCam.GetOrientation();
	CVector debugForward;
	CVector unused;
	CalculateControls(transform.forward(), debugForward, unused);

	return debugForward;
}

CVector CCamera::GetDebugCameraControlRight()
{
	CMatrix transform = mDebugCam.GetOrientation();
	CVector unused;
	CVector debugRight;
	CalculateControls(transform.forward(), unused, debugRight);

	return debugRight;
}

CQuaternion CCamera::GetDebugCameraOrientation()
{
	return mDebugCam.GetOrientation();
}
#endif