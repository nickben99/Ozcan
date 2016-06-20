//CameraLerpDistanceElevation.h - header for the camera lerp class

//system includes------------
//---------------------------

#ifndef _CameraLerpDistanceElevation_h_
#define _CameraLerpDistanceElevation_h_

//header files---------
#include "CameraLerp.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class CameraLerpDistanceElevation : public CameraLerp
{
public:
	CameraLerpDistanceElevation();
	CameraLerpDistanceElevation(float time, unsigned int collisionDetectDynamicTypeIn, unsigned int collisionDetectSpriteTypesIn);

	~CameraLerpDistanceElevation();

	virtual void Update(const CameraTransform& from, const CameraTransform& to);

	virtual CameraLerpDistanceElevation* CreateCopy();
}; // end class CameraLerpDistanceElevation

#endif // ifndef _CameraLerpDistanceElevation_h_ 