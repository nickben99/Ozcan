//CameraLerp.h - header for the camera lerp class

//system includes------------
//---------------------------

#ifndef _CameraLerp_h_
#define _CameraLerp_h_

//header files---------
#include "Camera\CameraTransform.h"
#include "Camera\CameraTransition.h"
#include "Camera\CameraTransitionCollision.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class CameraLerp
{
public:
	CameraLerp();
	CameraLerp(float time, unsigned int collisionDetectDynamicType, unsigned int collisionDetectSpriteTypes);
	~CameraLerp();

	void Start();
	float CalculateBlendInAmount();
	void DoCut();

	virtual void Update(const CameraTransform& from, const CameraTransform& to);

	virtual CameraLerp* CreateCopy();

	CameraTransform GetResult();

	bool IsTransitionFullyBlendedIn()
	{
		return CalculateBlendInAmount() >= 1.0f;
	}

	bool IsResolvingCollision()
	{
		return mTransitionCollision.IsResolvingCollision();
	}

	unsigned int GetCollisionDetectDynamicType() const
	{
		return collisionDetectDynamicType;
	}

	unsigned int GetCollisionDetectSpriteTypes() const
	{
		return collisionDetectSpriteTypes;
	}

protected:
	bool IsCollisionDetecting();

protected:
	float mTime;
	unsigned int collisionDetectDynamicType; 
	unsigned int collisionDetectSpriteTypes;
	
	float mChangeStartTime;

	CameraTransition mTransition;
	CameraTransitionCollision mTransitionCollision;

	CameraTransform mResult;
}; // end class CameraPriorityTypes

#endif // ifndef _CameraLerp_h_ 