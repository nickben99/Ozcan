//CameraTransform.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CameraTransform_h_
#define _CameraTransform_h_

//header files---------
#include "Math/CQuaternion.h"
#include "Math/CVector.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class CameraTransform
{
public:
	CameraTransform();

	void Init();

	void SetLookAt(const CVector& lookAt);
	CVector GetLookAt() const;

	void SetOrientation(const CQuaternion& orientation);
	CQuaternion GetOrientation() const;

	void SetDistanceFromLookAt(float distanceFromLookAt);
	float GetDistanceFromLookAt() const;

	CVector CalculatePosition() const;

	static CameraTransform Lerp(const CameraTransform& from, const CameraTransform& to, float interp);	

#ifdef DETERMINISTIC_REPLAY_DEBUG
	void ReplayCheck() const;
#endif

private:
	CVector mLookAt;
	CQuaternion mOrientation;
	float mDistanceFromLookAt;
}; // end class CameraTransform

#endif // ifndef _CameraTransform_h_ 