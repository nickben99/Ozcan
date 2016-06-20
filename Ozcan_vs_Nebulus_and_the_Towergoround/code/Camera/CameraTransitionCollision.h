//CameraTransitionCollision.h - header for the camera transition collision class

//system includes------------
//---------------------------

#ifndef _CameraTransitionCollision_h_
#define _CameraTransitionCollision_h_

//header files---------
#include "CameraTransform.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
class CameraLerp;
//------------------------

class CameraTransitionCollision
{
public:
	CameraTransitionCollision();
	~CameraTransitionCollision();

	void Update(const CameraTransform& transform, bool lookForCollisions, const CameraLerp& lerp, float minDist);
	CameraTransform GetResult();

	bool IsResolvingCollision()
	{
		return mIsColliding;
	}

	static float kCollisionRadius;

private:
	CameraTransform mResult;
	float mCurrentLength;
	bool mHasBegun;
	bool mIsColliding;
}; // end class CameraLinks

#endif // ifndef _CameraLinks_h_ 