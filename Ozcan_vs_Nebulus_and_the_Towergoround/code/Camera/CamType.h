// CamTypeTunnel.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CamType_h_
#define _CamType_h_

//header files---------
#include "Math/CVector.h"
//---------------------

//forward declerations----
class CameraTransform;
//------------------------

class CamType
{
	public:
		enum eCamIdentifier
		{
			eCICircleFollow = 0, 
			eCIReplay, 
			eCITunnel, 
			eCIMax
		};

		virtual void move(float perCentOfSecond) = 0;
		virtual void GetOutput(CameraTransform& transform) = 0;

		virtual eCamIdentifier GetType() = 0;
}; // end class

#endif // ifndef _CamType_h_ 