#if _DEBUG
// DebugCam.cpp

// header files -----------
#include "DebugCam.h"
#include "Game/Globals.h"
#include "Game/GameTime.h"
#include "Math/CMatrix.h"
#include "EularLerp.h"
#include "CCamera.h"

#include "Game/Globals.h"
//-------------------------

// extern ----------------
extern CCamera* theCamera; // from winSetup
// -----------------------

DebugCam::DebugCam()
	: mIsActive(false)
	, mIsPositionLocked(false)
{
}

void DebugCam::Init()
{
	mPosition = CCamera::instance()->GetGameCameraPosition();
	mOrientation = CCamera::instance()->GetGameCameraOrientation();
	mIsPositionLocked = false;
}

void DebugCam::Update()
{
	if(Globals::Instance().keys.GetKeyPressed(eKeyCode_C))
	{
		mIsActive = !mIsActive;
		if (mIsActive)
		{
			Init();
		}
	}

	static float kLinearSpeed = 10.0f;
	static float kLinearSpeedFastMultiplier = 4.0f;

	static float kOrientationSpeed = 1.0f;
	static float kOrientationSpeedFastMultiplier = 2.0f;

	if(Globals::Instance().keys.GetKeyPressed(eKeyCode_T))
	{
		mIsPositionLocked = !mIsPositionLocked;
	}

	if (mIsActive && !mIsPositionLocked)
	{
		float linearSpeed = kLinearSpeed;
		float orientationSpeed = kOrientationSpeed;
		if (Globals::Instance().keys.GetKeyStatus(eKeyCode_LSHIFT) || Globals::Instance().keys.GetKeyStatus(eKeyCode_RSHIFT))
		{
			linearSpeed *= kLinearSpeedFastMultiplier;
			orientationSpeed *= kOrientationSpeedFastMultiplier;
		}

		orientationSpeed *= GameTime::Instance().GetFixedTimeDeltaSeconds();
		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_J))
		{
			CQuaternion yRotation(orientationSpeed, CVector::unitY);
			mOrientation = yRotation * mOrientation;
			mOrientation.normalise();
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_L))
		{
			CQuaternion yRotation(-orientationSpeed, CVector::unitY);
			mOrientation = yRotation * mOrientation;
			mOrientation.normalise();
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_I))
		{
			CQuaternion xRotation(-orientationSpeed, CVector::unitX);
			mOrientation *= xRotation;
			mOrientation.normalise();
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_K))
		{
			CQuaternion xRotation(orientationSpeed, CVector::unitX);
			mOrientation *= xRotation;
			mOrientation.normalise();
		}

		CMatrix transformMat(mOrientation);
		linearSpeed *= GameTime::Instance().GetFixedTimeDeltaSeconds();
		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_R))
		{
			mPosition += transformMat.forward().v3 * linearSpeed;
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_F))
		{
			mPosition -= transformMat.forward().v3 * linearSpeed;
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_A))
		{
			mPosition += transformMat.right().v3 * linearSpeed;
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_D))
		{
			mPosition -= transformMat.right().v3 * linearSpeed;
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_E))
		{
			mPosition += CVector::unitY * linearSpeed;
		}

		if(Globals::Instance().keys.GetKeyStatus(eKeyCode_Q))
		{
			mPosition -= CVector::unitY * linearSpeed;
		}

		CVector forward = transformMat.forward();
		if (CVector::SafeNormalizeXZ(forward))
		{
			if(Globals::Instance().keys.GetKeyStatus(eKeyCode_W))
			{
				mPosition += forward * linearSpeed;
			}

			if(Globals::Instance().keys.GetKeyStatus(eKeyCode_S))
			{
				mPosition -= forward * linearSpeed;
			}
		}
	}
}

void DebugCam::GetOutput(CVector& outPosition, CQuaternion& outOrientation)
{
	outPosition = mPosition;
	outOrientation = mOrientation;
}

bool DebugCam::IsActive()
{
	return mIsActive;
}

bool DebugCam::IsPositionLocked()
{
	return mIsPositionLocked;
}

#endif
