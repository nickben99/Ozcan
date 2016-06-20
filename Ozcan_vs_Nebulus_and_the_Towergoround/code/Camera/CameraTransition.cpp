//CameraTransition.cpp, the camera transition class

// system includes --------
//-------------------------

// header files -----------
#include "CameraTransition.h"
#include "EularLerp.h"
//-------------------------

CameraTransition::CameraTransition()
	: mIsSavedLerpBeingUsed(false)
	, mHasTransitionBegun(false)
{
}

CameraTransition::~CameraTransition()
{
}

void CameraTransition::Update(const CameraTransform& from, const CameraTransform& to, float interp)
{
	EularLerp fromEular(from.GetOrientation());
	EularLerp toEular(to.GetOrientation());
	EularLerp lerpDirection = toEular-fromEular;

	if (interp >= 1.0f)
	{
		mResult = to;
		return;
	}

	if (!mHasTransitionBegun)
	{
		mSavedLerpDirection = lerpDirection;
		mHasTransitionBegun = true;		
		mResult = CameraTransform::Lerp(from, to, interp);	
	}
	else
	{
		if (!mIsSavedLerpBeingUsed)
		{			
			const float NearSideDirectionChange = PI*0.25f;
			if (mSavedLerpDirection.GetYaw() * lerpDirection.GetYaw() < 0.0f && // the direction has changed
				(fabs(mSavedLerpDirection.GetYaw()) > NearSideDirectionChange || fabs(lerpDirection.GetYaw()) > NearSideDirectionChange)) // not a near side change
			{
				mIsSavedLerpBeingUsed = true;
			}
		}

		if (mIsSavedLerpBeingUsed)
		{		
			CameraTransform savedTransform = from;
			// this line is not needed, as mResult.SetOrientation() is done after CameraTransform::Lerp
			//savedTransform.SetOrientation((toEular - mSavedLerpDirection).CalculateOrientation());

			//{ // sanity checks
			//	EularLerp savedFromEular = (toEular - mSavedLerpDirection);
			//	EularLerp fixedLerpDirection = toEular-savedFromEular;
			//	if (fixedLerpDirection.GetYaw() * mSavedLerpDirection.GetYaw() < 0.0f)
			//	{
			//		fixedLerpDirection = fixedLerpDirection;
			//	}
			//}

			//{ // sanity checks
			//	EularLerp savedFromEular(savedTransform.GetOrientation());
			//	EularLerp fixedLerpDirection = toEular-savedFromEular;
			//	if (fixedLerpDirection.GetYaw() * mSavedLerpDirection.GetYaw() < 0.0f)
			//	{
			//		fixedLerpDirection = fixedLerpDirection;
			//	}
			//}

			mResult = CameraTransform::Lerp(savedTransform, to, interp);	
			// this line was added as the lerp between cameras would change direction mid lerp when leaving tunnels
			// it was caused by slight inacuracy when converting between eular angles and quaternions, when the angle to lerp
			// between was very close to 180 degrees
			mResult.SetOrientation(EularLerp::Lerp(toEular - mSavedLerpDirection, toEular, interp).CalculateOrientation());

			//{ // sanity checks
			//	EularLerp savedFromEular(savedTransform.GetOrientation());
			//	EularLerp resultEular(mResult.GetOrientation());
			//	EularLerp resultLerpDirection = resultEular-savedFromEular;
			//	if (resultLerpDirection.GetYaw() * mSavedLerpDirection.GetYaw() < 0.0f)
			//	{
			//		resultLerpDirection = resultLerpDirection;
			//	}

			//	EularLerp fixedLerpDirection = toEular-resultEular;
			//	if (fixedLerpDirection.GetYaw() * mSavedLerpDirection.GetYaw() < 0.0f)
			//	{
			//		fixedLerpDirection = fixedLerpDirection;
			//	}
			//}
		}
		else
		{
			mSavedLerpDirection = lerpDirection;
			mResult = CameraTransform::Lerp(from, to, interp);		
		}
	}
}

CameraTransform CameraTransition::GetResult()
{
	return mResult;
}
