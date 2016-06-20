#if _DEBUG
// DebugCam.h

//system includes------------
//---------------------------

#ifndef _DebugCam_h_
#define _DebugCam_h_

//header files---------
#include "Math/CVector.h"
#include "Math/CQuaternion.h"
//---------------------

//forward declerations----
//------------------------

class DebugCam
{
public:
	DebugCam();

	void Init();
	void Update();
	void GetOutput(CVector& outPosition, CQuaternion& outOrientation);

	CVector GetPosition()
	{
		return mPosition;
	}

	CQuaternion GetOrientation()
	{
		return mOrientation;
	}

	bool IsActive();
	bool IsPositionLocked();

private:
	CVector mPosition;
	CQuaternion mOrientation;

	bool mIsActive;
	bool mIsPositionLocked;
}; // end class DebugCam

#endif // ifndef _DebugCam_h_ 
#endif // _DEBUG