// CamTypeReplay.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CamTypeReplay_h_
#define _CamTypeReplay_h_

//header files---------
#include "CamType.h"
//---------------------

//forward declerations----
//------------------------

class CamTypeReplay : public CamType
{
	private:
//-----private variables-----------
		CVector position;
//---------------------------------

	public:
		CamTypeReplay(); // constructor

//----public variables--------------		
//----------------------------------

//----public methods----------------------------------------
		void initialise(); // do any initialisation
		// move the camera into position
		void move(float perCentOfSecond);

		CVector GetLookAt();

		CVector GetPosition()
		{
			return position;
		}

		virtual void GetOutput(CameraTransform& transform);

		virtual eCamIdentifier GetType(){ return CamType::eCIReplay; };

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CamTypeReplay

#endif // ifndef _CamTypeReplay_h_ 