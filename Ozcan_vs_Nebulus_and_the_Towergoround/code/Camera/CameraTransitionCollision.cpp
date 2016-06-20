//CameraTransitionCollision.cpp, the camera transition collision class

// system includes --------
//-------------------------

// header files -----------
#include "CameraTransitionCollision.h"
#include "Game/CLevel.h"
#include "Game/Globals.h"
#include "Game/defines.h"
//-------------------------

float CameraTransitionCollision::kCollisionRadius = 0.5f;//0.1f;

CameraTransitionCollision::CameraTransitionCollision()
	: mCurrentLength(1.0f)
	, mHasBegun(false)
	, mIsColliding(false)
{
}

CameraTransitionCollision::~CameraTransitionCollision()
{
}

void CameraTransitionCollision::Update(const CameraTransform& transform, bool lookForNewCollisions, const CameraLerp& lerp, float minDist)
{
	mResult = transform;

	CVector hitPoint;
	CVector hitNormal;

	if (!mHasBegun)
	{
		mHasBegun = true;				
		if (lookForNewCollisions && 
			Globals::Instance().currentLevel.theScene.SphereCast(transform.GetLookAt(), 
																transform.CalculatePosition(), 
																CameraTransitionCollision::kCollisionRadius, 
																lerp.GetCollisionDetectDynamicType(), lerp.GetCollisionDetectSpriteTypes(), 
																hitPoint, hitNormal))
		{
			mCurrentLength = defines::Max(minDist, (hitPoint-transform.GetLookAt()).getMagnitude());
			mIsColliding = true;
			mResult.SetDistanceFromLookAt(mCurrentLength);
		}
		else
		{
			mCurrentLength = (transform.CalculatePosition()-transform.GetLookAt()).getMagnitude();
		}
	}
	else
	{
		float desiredLength = 1.0f;
		bool collidedThisFrame = false;		
		if (lookForNewCollisions && Globals::Instance().currentLevel.theScene.SphereCast(transform.GetLookAt(), 
																						transform.CalculatePosition(), 
																						CameraTransitionCollision::kCollisionRadius, 
																						lerp.GetCollisionDetectDynamicType(), lerp.GetCollisionDetectSpriteTypes(), 
																						hitPoint, hitNormal))
		{
			desiredLength = defines::Max(minDist, (hitPoint-transform.GetLookAt()).getMagnitude());
			mIsColliding = true;
			collidedThisFrame = true;
		}
		else
		{
			desiredLength = (transform.CalculatePosition()-transform.GetLookAt()).getMagnitude();
		}

		static float kLerpIn = 30.0f;
		static float kLerpOut = 2.0f;

		if (mIsColliding)
		{
			float diff = (desiredLength - mCurrentLength);
			float mult = 0.0f;
			if (desiredLength < mCurrentLength)
			{				
				mult = defines::Min(1.0f, kLerpIn * GameTime::Instance().GetTimeDeltaSeconds());
			}
			else
			{
				mult = defines::Min(1.0f, kLerpOut * GameTime::Instance().GetTimeDeltaSeconds());				
			}
			mCurrentLength += diff * mult;

			if (!collidedThisFrame && fabs(mCurrentLength-desiredLength) < 0.001f)
			{
				mIsColliding = false;
			}
		}

		if (!mIsColliding)
		{
			mCurrentLength = desiredLength;			
		}
		mResult.SetDistanceFromLookAt(mCurrentLength);
	}	
}

CameraTransform CameraTransitionCollision::GetResult()
{
	return mResult;
}
