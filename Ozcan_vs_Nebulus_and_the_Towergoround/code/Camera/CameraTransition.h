//CameraTransition.h - header for the camera transition class

//system includes------------
//---------------------------

#ifndef _CameraTransition_h_
#define _CameraTransition_h_

//header files---------
#include "CameraTransform.h"
#include "EularLerp.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class CameraTransition
{
public:
	CameraTransition();
	~CameraTransition();

	//void BeginTransition();
	void Update(const CameraTransform& from, const CameraTransform& to, float interp);
	CameraTransform GetResult();

private:
	CameraTransform mResult;
	EularLerp mSavedLerpDirection;
	bool mIsSavedLerpBeingUsed;
	bool mHasTransitionBegun;
}; // end class CameraLinks

#endif // ifndef _CameraLinks_h_ 