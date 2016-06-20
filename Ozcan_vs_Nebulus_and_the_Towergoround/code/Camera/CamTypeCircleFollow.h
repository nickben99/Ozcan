//CCamera.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CamTypeCircleFollow_h_
#define _CamTypeCircleFollow_h_

//header files---------
#include "Game/CSprite.h"
#include "Game/CNebulus.h"
#include "CamType.h"
//---------------------

//forward declerations----
//------------------------

class CamTypeCircleFollow : public CamType
{
	private:
//-----private variables-----------
		CNebulus* theNebulus; // pointer to Nebulus

		CSprite cameraPosition, 
				cameraPositionY,
				sceneCentre; // different sprites used as springs for the camera

		CSprite position;

		float FAR_OUTnebulusOffset;
		float FAR_OUTcameraPositionToSceneCentreRestLength;
		float FAR_OUTrelativeCameraHeight;
//---------------------------------

	private:
		void SetSpritePosition();

	public:
		CamTypeCircleFollow(); // constructor

//----public variables--------------		
		float	sceneCentreToNebulusSpringConstant,
				sceneCentreToNebulusFrictionConstant, 
				sceneCentreToNebulusRestLength,
				cameraPositionYSpringConstant,
				cameraPositionYFrictionConstant, 
				cameraPositionYRestLength,
				cameraPositionToSceneCentreSpringConstant, 
				cameraPositionToSceneCentreFrictionConstant, 
				cameraPositionToSceneCentreRestLength, 
				// how far above the scene centre the camera position is
				relativeCameraHeight,

				// nebulus offset on the y axis
				nebulusOffset;
//----------------------------------

//----public methods----------------------------------------
		void initialise(); // do any initialisation
		// move the camera into position
		void move(float perCentOfSecond);

		CVector GetLookAt();
		CVector GetPosition()
		{
			return position.position;
		}
		virtual void GetOutput(CameraTransform& transform);

		virtual eCamIdentifier GetType(){ return CamType::eCICircleFollow; };

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CCamera

#endif // ifndef _CCamera_h_ 