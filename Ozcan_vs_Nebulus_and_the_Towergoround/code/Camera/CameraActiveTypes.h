//CameraActiveTypes.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CameraActiveTypes_h_
#define _CameraActiveTypes_h_

//header files---------
#include "CamType.h"
#include "Math/CQuaternion.h"
#include "Math/CVector.h"
#include "Game/GameTime.h"
#include "CamType.h"
#include "CameraTransform.h"
#include "Game/defines.h"
#include "Camera/CameraTransition.h"
#include "Camera/CameraTransitionCollision.h"
#include "Camera/CameraLerp.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class CameraActiveTypes
{
public:
	CameraActiveTypes();
	~CameraActiveTypes();

	void Initialize();

	CamType::eCamIdentifier GetNewestCameraType();
	// we want to change to a new type of camera
	void ChangeTo(CamType* pType, CameraLerp* pLerp);
	void Update();
	void GetOutput(CameraTransform& outTransform);

private:
	static const int kMaxSimultaneousTypes = 10;

	struct TypeContainer
	{
		TypeContainer()
			: mpCamType(0)
		{}

		bool IsTransitionAndCollisionFullyBlendedIn()
		{
			return mpLerp->CalculateBlendInAmount() >= 1.0f && !mpLerp->IsResolvingCollision();
		}
		
		CamType* mpCamType;
		CameraLerp* mpLerp;
	};

private:
	void DestroyAll();
	void CameraActiveTypes::Destroy(TypeContainer& container);
	void ClearSpaceForNewType();
	bool IsSpaceAvailable();
	void DeleteOldestActiveType();
	void Cull();
	int GetNewestCameraIndex();

private:
	TypeContainer mActiveTypes[kMaxSimultaneousTypes];
	int mCurrentNumActiveTypes;

	CameraTransform mTransform;
}; // end class CameraActiveTypes

#endif // ifndef _CameraActiveTypes_h_ 