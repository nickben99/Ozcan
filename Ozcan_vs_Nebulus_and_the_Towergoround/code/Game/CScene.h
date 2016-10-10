//CScene.h - header for the scene class

//system includes------------
//---------------------------

#ifndef _CScene_h_
#define _CScene_h_

#ifdef _WINDOWS

//header files---------
#include "CTower.h"
#endif //_WINDOWS
#include "Camera/CCamera.h" // the scenes camera
#ifdef _WINDOWS
#include "CSkyBoard.h"
#include "CFrustum.h"
#include "CSpriteEngine.h"
//---------------------
#endif //_WINDOWS
//defines--------------
//---------------------

//external definitions-----
//-------------------------

class CScene
{
#ifdef _WINDOWS
	private:
//-----private variables-----------
#ifdef USE_SHADERS
		int ambientColorUniform;
		int pointLightingLocationUniform;
		int pointLightingSpecularColorUniform;
		int pointLightingDiffuseColorUniform;
#endif
//---------------------------------

		bool ShapeCast(const CVector& from, const CVector& to, float radius, unsigned int dynamicTypes, unsigned int spriteTypes, CVector& outHitPoint, CVector& outHitNormal);
#endif // _WINDOWS
	public:
//----public variables--------------
		CTower * tower;
		CCamera* theCamera; // the scenes camera
#ifdef _WINDOWS
		CSkyBoard skyBox; // the scenes sky box

		CFrustum *theFrustum; // pointer to the singleton frustum
		CSpriteEngine* theSprites;
//----------------------------------

//----public methods----------------------------------------
		CScene(); // constructor
		~CScene(); // destructor
		void deleteCurrentScene(); // deletes any memory associated with the current scene
		bool initialise(char * backgroundTexture); // do any initialisation
		// position the scene
		void positionCamera(GLfloat timeChangeInSeconds);
#endif
		CVector4 GetLightPosition();
		CVector4 GetLightDirection();

		// do a ray cast against the scene triangles
		bool RayCast(const CVector& from, const CVector& to, unsigned int dynamicTypes, unsigned int spriteTypes, CVector& outHitPoint, CVector& outHitNormal);

		// do a sphere cast against the scene triangles
		bool SphereCast(const CVector& from, const CVector& to, float radius, unsigned int dynamicTypes, unsigned int spriteTypes, CVector& outHitPoint, CVector& outHitNormal);

		// overloaded operators

//-----------------------------------------------------------
}; // end class CScene

#endif // ifndef _CScene_h_