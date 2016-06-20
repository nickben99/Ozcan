//CameraTransform.cpp, the camera class

// system includes --------
//-------------------------

// header files -----------
#include "CameraTransform.h"
#include "Math/CMatrix.h"
#include "EularLerp.h"
#include "Game/ReplayManager.h"
//-------------------------

CameraTransform::CameraTransform()
	: mDistanceFromLookAt(1.0f)
{
}

void CameraTransform::Init()
{
	mDistanceFromLookAt = 1.0f;
	mLookAt.reset();
	mOrientation.identity();
}

void CameraTransform::SetLookAt(const CVector& lookAt)
{
	mLookAt = lookAt;
}

CVector CameraTransform::GetLookAt() const
{
	return mLookAt;
}

void CameraTransform::SetOrientation(const CQuaternion& orientation)
{
	mOrientation = orientation;
}

CQuaternion CameraTransform::GetOrientation() const
{
	return mOrientation;
}

void CameraTransform::SetDistanceFromLookAt(float distanceFromLookAt)
{
	mDistanceFromLookAt = distanceFromLookAt;
}

float CameraTransform::GetDistanceFromLookAt() const
{
	return mDistanceFromLookAt;
}

CVector CameraTransform::CalculatePosition() const
{
	return mLookAt - (CMatrix(mOrientation).forward().v3 * mDistanceFromLookAt);
}

CameraTransform CameraTransform::Lerp(const CameraTransform& from, const CameraTransform& to, float interp)
{
	CameraTransform result;
	result.SetDistanceFromLookAt(from.mDistanceFromLookAt+((to.mDistanceFromLookAt-from.mDistanceFromLookAt)*interp));
	result.SetLookAt(from.mLookAt+((to.mLookAt-from.mLookAt)*interp));
	result.SetOrientation(EularLerp::Lerp(from.mOrientation, to.mOrientation, interp));
	return result;	
}

#ifdef DETERMINISTIC_REPLAY_DEBUG
void CameraTransform::ReplayCheck() const
{
	ReplayManager::instance()->VectorResult(mLookAt);
	ReplayManager::instance()->QuaternionResult(mOrientation);
	ReplayManager::instance()->FloatResult(mDistanceFromLookAt);
}
#endif
