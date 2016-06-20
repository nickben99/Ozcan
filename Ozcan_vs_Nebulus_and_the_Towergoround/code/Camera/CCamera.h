//CCamera.h - header for the camera class

//system includes------------
//---------------------------

#ifndef _CCamera_h_
#define _CCamera_h_

//header files---------
#include "Game/CSprite.h"
#include "Game/CNebulus.h"
#include "Game/CTower.h"
#include "Math/CMatrix.h"
#include "Game/defines.h"
#include "CamTypeCircleFollow.h"
#include "CamTypeReplay.h"
#include "CamTypeTunnel.h"
#include "Camera/CameraActiveTypes.h"
#include "Camera/CameraPriorityTypes.h"
#include "Camera/CameraLinks.h"
#if _DEBUG
#include "DebugCam.h"
#endif
//---------------------

//defines--------------
enum cameraModes
{
	FAR_OUT,
	CLOSE_UP, 
	STATIC_REPLAY, 
	TUNNEL_CAM
};
//---------------------

//forward declerations----
//------------------------

class CCamera
{
	private:
//-----private variables-----------
		CameraActiveTypes mActiveTypes;
		CameraLinks mLinks;
		CNebulus* theNebulus; // pointer to Nebulus		
		CamTypeCircleFollow mFollowCam;
		CamTypeReplay mStaticReplay;
		//CamTypeTunnel mTunnelCam;

		CMatrix rotation;
		CVector position, forwardControl, rightControl; /*	the forward and right vector of the cameras orientation (used when moving Nebulus) */
		CVector lookAt;

		CMatrix modelViewMatrix;

#if _DEBUG
		DebugCam mDebugCam;
#endif
//---------------------------------

//-----private methods-------------
		CCamera(); // constructor
		// during a camera change over period lerp to the new values
		void lerpToNewCameraValues(GLfloat perCentOfSecond);

		void TryChangeCamera(bool doReplayChecks);
//---------------------------------

	public:
//----public variables--------------		
		CameraPriorityTypes mPriorityTypes;
		// the mode the camera is in (from enum above)
		cameraModes cameraMode;
		int changingCameraMode; // flag to determine if the camera is in a transition period

		// CLOSE_UP camera view variables
		float	currentYRotation, 
				CLOSE_UPnebulusOffset,
				CLOSE_UPcameraPositionToSceneCentreRestLength,
				CLOSE_UPrelativeCameraHeight,

				// FAR_OUT camera view variables
				FAR_OUTnebulusOffset,
				FAR_OUTcameraPositionToSceneCentreRestLength,
				FAR_OUTrelativeCameraHeight,

				// how long the camera takes to change from one view to another
				cameraChangePeriod,
				cameraChangeTimer, // records how much of the camera change period has elapsed

				/*	when the camera changes views, these variables record how much the 
					camera variables should change over the cameraChangePeriod */
				nebulusOffsetLERP_LENGTH,
				cameraPositionToSceneCentreRestLengthLERP_LENGTH,
				relativeCameraHeightLERP_LENGTH;
//----------------------------------

//----public methods----------------------------------------
		static CCamera* instance(); // get the singleton camera instance
		~CCamera(); // destructor
		void initialise(); // do any initialisation
		// set the camera mode
		void setCameraMode(cameraModes modeRequired);
		// move the camera into position
		void Update();
		void SetCameraOutputVariables(bool doReplayChecks);
		void SetCamera();

		CVector GetActiveCameraPosition();

		CVector GetGameCameraPosition();
		CVector GetGameCameraControlForward();
		CVector GetGameCameraControlRight();
		CQuaternion GetGameCameraOrientation();
		CMatrix GetGameCameraSkyBoardRotation();

#ifdef USE_SHADERS
		inline CMatrix GetModelViewMatrix()
		{
			return modelViewMatrix;
		};
#endif

#if _DEBUG
		bool IsDebugCamActive();
		bool IsDebugCamPositionLocked();
		CVector GetDebugCameraPosition();
		CVector GetDebugCameraControlForward();
		CVector GetDebugCameraControlRight();
		CQuaternion GetDebugCameraOrientation();
#endif

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CCamera

#endif // ifndef _CCamera_h_ 