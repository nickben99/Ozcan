//CameraActiveTypes.cpp, the camera class

// system includes --------
#include <math.h>
#include <assert.h>
//-------------------------

// header files -----------
#include "CameraActiveTypes.h"
#include "Math/CMatrix.h"
#include "EularLerp.h"
#include "Game/defines.h"
#include "CameraTransform.h"
#include "Camera/CameraLerp.h"
#include "Camera/CameraTransitionCollision.h"
#include "Game/ReplayManager.h"
//-------------------------

const int kOldestIndex = 0;

CameraActiveTypes::CameraActiveTypes()
	: mCurrentNumActiveTypes(0)
{
}

CameraActiveTypes::~CameraActiveTypes()
{
	DestroyAll();
}

void CameraActiveTypes::Initialize()
{
	DestroyAll();
	mTransform.Init();
}

void CameraActiveTypes::DestroyAll()
{
	for (int active = 0; active < kMaxSimultaneousTypes; ++active)
	{
		Destroy(mActiveTypes[active]);
	}
	mCurrentNumActiveTypes = 0;
}

void CameraActiveTypes::Destroy(TypeContainer& container)
{	
	if (0 != container.mpCamType)
	{
		delete container.mpCamType;
		container.mpCamType = 0;
	}

	if (0 != container.mpLerp)
	{
		delete container.mpLerp;
		container.mpLerp = 0;
	}
}

CamType::eCamIdentifier CameraActiveTypes::GetNewestCameraType()
{
	if (GetNewestCameraIndex() >= 0)
	{
		return mActiveTypes[GetNewestCameraIndex()].mpCamType->GetType();
	}
	return CamType::eCIMax;
}

void CameraActiveTypes::ChangeTo(CamType* pType, CameraLerp* pLerp)
{
	ClearSpaceForNewType(); // make sure we have room for a new type
	assert(0 == mActiveTypes[mCurrentNumActiveTypes].mpCamType);
	assert(0 == mActiveTypes[mCurrentNumActiveTypes].mpLerp);
	
	mActiveTypes[mCurrentNumActiveTypes].mpCamType = pType;
	mActiveTypes[mCurrentNumActiveTypes].mpLerp = pLerp;
	mActiveTypes[mCurrentNumActiveTypes].mpLerp->Start();

	if (0 == mCurrentNumActiveTypes)
	{
		mActiveTypes[mCurrentNumActiveTypes].mpLerp->DoCut();
	}

	if (0 == mCurrentNumActiveTypes)
	{
		pType->GetOutput(mTransform);	// doing this here means the camera is in the correct position while we are paused, awaiting
										// the first frame
	}
	++mCurrentNumActiveTypes;
}

void CameraActiveTypes::Update()
{
#ifdef DETERMINISTIC_REPLAY_DEBUG		
	ReplayManager::instance()->IntResult(mCurrentNumActiveTypes);		
#endif
	if (mCurrentNumActiveTypes> 0)
	{
		for (int index = kOldestIndex; index < mCurrentNumActiveTypes; ++index)
		{			
			mActiveTypes[index].mpCamType->move(GameTime::Instance().GetTimeDeltaSeconds());
			
			CameraTransform output;
			mActiveTypes[index].mpCamType->GetOutput(output);

#ifdef DETERMINISTIC_REPLAY_DEBUG
			output.ReplayCheck();		
#endif

			mActiveTypes[index].mpLerp->Update(mTransform, output);
			mTransform = mActiveTypes[index].mpLerp->GetResult();	

#ifdef DETERMINISTIC_REPLAY_DEBUG
			mTransform.ReplayCheck();		
#endif
		}
		Cull();
	}
}

// delete old cameras which are no longer contributing to the output
void CameraActiveTypes::Cull()
{
	for (int active = 0; active < mCurrentNumActiveTypes; ++active)
	{
		if (0 == mActiveTypes[active].mpCamType || 0 == mActiveTypes[active].mpLerp)
		{
			assert(false);
		}
	}

	int newestFullyBlendedCamera = -1;
	for (int index = GetNewestCameraIndex(); index > kOldestIndex; --index)
	{
		if (mActiveTypes[index].IsTransitionAndCollisionFullyBlendedIn())
		{
			newestFullyBlendedCamera = index;
			mCurrentNumActiveTypes -= newestFullyBlendedCamera;
			break;
		}
	}	

	if (-1 != newestFullyBlendedCamera)
	{
		for (int active = 0; active < mCurrentNumActiveTypes + newestFullyBlendedCamera; ++active)
		{
			Destroy(mActiveTypes[active]);
				
			if (active < mCurrentNumActiveTypes)
			{
				mActiveTypes[active] = mActiveTypes[active + newestFullyBlendedCamera];
				mActiveTypes[active + newestFullyBlendedCamera].mpCamType = 0;			
				mActiveTypes[active + newestFullyBlendedCamera].mpLerp = 0;		
			}
		}
	}

	for (int active = 0; active < mCurrentNumActiveTypes; ++active)
	{
		if (0 == mActiveTypes[active].mpCamType || 0 == mActiveTypes[active].mpLerp)
		{
			assert(false);
		}
	}
}

int CameraActiveTypes::GetNewestCameraIndex()
{
	return mCurrentNumActiveTypes - 1;
}

void CameraActiveTypes::GetOutput(CameraTransform& outTransform)
{
	outTransform = mTransform;
}

void CameraActiveTypes::ClearSpaceForNewType()
{
	for (int active = 0; active < mCurrentNumActiveTypes; ++active)
	{
		if (0 == mActiveTypes[active].mpCamType || 0 == mActiveTypes[active].mpLerp)
		{
			assert(false);
		}
	}

	if (!IsSpaceAvailable()) // if there is no room left
	{
		DeleteOldestActiveType();
		// shuffle all the cam types down
		for (int active = kOldestIndex; active < mCurrentNumActiveTypes-1; ++active)
		{
			mActiveTypes[active] = mActiveTypes[active + 1];
			mActiveTypes[active + 1].mpCamType = 0;
			mActiveTypes[active + 1].mpLerp = 0;
		}
		--mCurrentNumActiveTypes;
	}

	for (int active = 0; active < mCurrentNumActiveTypes; ++active)
	{
		if (0 == mActiveTypes[active].mpCamType || 0 == mActiveTypes[active].mpLerp)
		{
			assert(false);
		}
	}
}

bool CameraActiveTypes::IsSpaceAvailable()
{
	return (mCurrentNumActiveTypes < kMaxSimultaneousTypes);
}

void CameraActiveTypes::DeleteOldestActiveType()
{	
	Destroy(mActiveTypes[kOldestIndex]);
}