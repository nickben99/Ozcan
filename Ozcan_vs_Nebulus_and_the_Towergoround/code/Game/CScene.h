//CScene.h - header for the scene class

//system includes------------
//---------------------------

#ifndef _CScene_h_
#define _CScene_h_

//header files---------
#include "CTower.h"
#include "Camera/CCamera.h" // the scenes camera
#include "CSkyBoard.h"
#include "CFrustum.h"
#include "CSpriteEngine.h"
//---------------------

//defines--------------
//---------------------

//external definitions-----
//-------------------------

class CScene
{
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

	public:
//----public variables--------------
		CTower * tower;
		CCamera* theCamera; // the scenes camera
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