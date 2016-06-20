//CameraLerp.cpp, the camera lerp class

// system includes --------
//-------------------------

// header files -----------
#include "CameraLerp.h"
#include "Game\GameTime.h"
#include "Game\defines.h"
#include "Camera\CameraTransform.h"
#ifdef DETERMINISTIC_REPLAY_DEBUG
#include "Game\ReplayManager.h"
#endif
//-------------------------

CameraLerp::CameraLerp()
	: mTime(1.0f)
	, collisionDetectDynamicType(0)
	, collisionDetectSpriteTypes(0)
	, mChangeStartTime(0.0f)
{
}

CameraLerp::CameraLerp(float time, unsigned int collisionDetectDynamicTypeIn, unsigned int collisionDetectSpriteTypesIn)
	: mTime(time)
	, collisionDetectDynamicType(collisionDetectDynamicTypeIn)
	, collisionDetectSpriteTypes(collisionDetectSpriteTypesIn)
	, mChangeStartTime(0.0f)
{
}

CameraLerp::~CameraLerp()
{
}

void CameraLerp::Start()
{
	mChangeStartTime = GameTime::Instance().GetElapsed();
#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->FloatResult(mChangeStartTime);
#endif
	mTransition = CameraTransition();
	mTransitionCollision = CameraTransitionCollision();
}

void CameraLerp::Update(const CameraTransform& from, const CameraTransform& to)
{
	float blendInAmt = CalculateBlendInAmount(); 
	float moderatedBlendInAmt = SlowFastSlowLerp(blendInAmt); 
#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->FloatResult(blendInAmt);
	ReplayManager::instance()->FloatResult(moderatedBlendInAmt);
#endif
	mTransition.Update(from, to, moderatedBlendInAmt); 
	mResult = mTransition.GetResult();	

#ifdef DETERMINISTIC_REPLAY_DEBUG
	mResult.ReplayCheck();		
#endif

	if (IsCollisionDetecting())
	{
		const float kNoMinDist = -1.0f;
		mTransitionCollision.Update(mResult, !IsTransitionFullyBlendedIn(), *this, kNoMinDist); 
		mResult = mTransitionCollision.GetResult();	
	}
}

CameraTransform CameraLerp::GetResult()
{
	return mResult;
}

void CameraLerp::DoCut()
{
	mTime = 0.0f;
}

float CameraLerp::CalculateBlendInAmount()
{
	float elapsed = GameTime::Instance().GetElapsed() - mChangeStartTime;
	float blend = mTime > 0.0f ? elapsed / mTime : 1.0f;

#ifdef DETERMINISTIC_REPLAY_DEBUG
	ReplayManager::instance()->FloatResult(GameTime::Instance().GetElapsed());
	ReplayManager::instance()->FloatResult(mChangeStartTime);
	ReplayManager::instance()->FloatResult(elapsed);
	ReplayManager::instance()->FloatResult(mTime);
	ReplayManager::instance()->FloatResult(blend);
#endif
	return defines::Min(1.0f, blend);
}

bool CameraLerp::IsCollisionDetecting()
{
	return 0 != collisionDetectDynamicType && 0 != collisionDetectSpriteTypes;
}

CameraLerp* CameraLerp::CreateCopy()
{
	return new CameraLerp(mTime, collisionDetectDynamicType, collisionDetectSpriteTypes);
}