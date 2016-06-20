//CameraLerpDistanceElevation.cpp, the camera lerp class

// system includes --------
//-------------------------

// header files -----------
#include "CameraLerpDistanceElevation.h"
//-------------------------

CameraLerpDistanceElevation::CameraLerpDistanceElevation()
	: CameraLerp()
{
}

CameraLerpDistanceElevation::CameraLerpDistanceElevation(float time, unsigned int collisionDetectDynamicTypeIn, unsigned int collisionDetectSpriteTypesIn)
	: CameraLerp(time, collisionDetectDynamicTypeIn, collisionDetectSpriteTypesIn)
{
}

CameraLerpDistanceElevation::~CameraLerpDistanceElevation()
{
}

void CameraLerpDistanceElevation::Update(const CameraTransform& from, const CameraTransform& to)
{
	mTransition.Update(from, to, SlowFastSlowLerp(CalculateBlendInAmount())); 
	mResult = mTransition.GetResult();	
	
	if (IsCollisionDetecting())
	{
		const float kMinDist = 1.0f;
		mTransitionCollision.Update(mResult, !IsTransitionFullyBlendedIn(), *this, kMinDist); 
		mResult = mTransitionCollision.GetResult();	
	}
}

CameraLerpDistanceElevation* CameraLerpDistanceElevation::CreateCopy()
{
	return new CameraLerpDistanceElevation(mTime, collisionDetectDynamicType, collisionDetectSpriteTypes);
}