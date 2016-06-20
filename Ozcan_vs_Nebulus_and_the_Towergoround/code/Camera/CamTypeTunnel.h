// CamTypeTunnel.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CamTypeTunnel_h_
#define _CamTypeTunnel_h_

//header files---------
#include "CamType.h"
#include "Math/CVector.h"
#include "Game/CTunnel.h"
#include "CamType.h"
//---------------------

//forward declerations----
//------------------------

class CamTypeTunnel : public CamType
{
	private:
//-----private variables-----------
		CTunnel* mpTunnel; // the tunnel nebulus is currently in
		CVector lookAt;
		CVector offset;
		float orientation;
		float orientationSpeed;
		CVector position;
//---------------------------------

	public:
		CamTypeTunnel(); // constructor

//----public variables--------------		
//----------------------------------

//----public methods----------------------------------------
		void initialise(); // do any initialisation
		// move the camera into position
		void move(float perCentOfSecond);

		CVector GetPosition()
		{
			return position; 
		}
		virtual void GetOutput(CameraTransform& transform);

		virtual eCamIdentifier GetType(){ return CamType::eCITunnel; };

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CamTypeTunnel

#endif // ifndef _CamTypeTunnel_h_ 