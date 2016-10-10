//CSkyBoard.h - header for the sky board class

//system includes------------
#include "Rendering/OpenGLInclude.h"
//---------------------------

#ifndef _CSkyBoard_h_
#define _CSkyBoard_h_

//header files---------
#include "CSprite.h"
#include "Camera/CCamera.h"
#include "Math/CMatrix.h"
#include "CTower.h"
#include "Game/defines.h"
#ifdef USE_SHADERS
#include "Rendering/MeshBuffer.h"
#endif
//---------------------

//defines--------------
//---------------------

class CSkyBoard : public CSprite
{
	private:
//-----private variables-----------
#ifdef USE_SHADERS	
		MeshBuffer meshBuffer;
#endif
//---------------------------------

	public:
//----public variables--------------
		CCamera * theCamera; // pointer to the singleton camera instance
		CTower * theTower;  // pointer to the singleton tower instance

		int skyTex; // the texture for the sky background
				
				// sky board dimensions
		float	fullLength, 
				leftSide, 
				rightSide,
				distToBillboard,
				top,
				bottom,
				// texture coordinates
				perCentOfTexLeft, 
				perCentOfTexRight, 
				normalTexDist,
				texOffset;
//----------------------------------

//----public methods----------------------------------------
		CSkyBoard(); // constructor
		CSkyBoard(CModel * aModel); // overloaded constructor 
		// move the sky box into position
		void move(GLfloat perCentOfSecond);
		// draw the sky box
		void draw();
		~CSkyBoard(); // destructor
		bool initialise(char * backgroundTexture); // initialisation
		// delete sky board
		void deleteSkyBoard();

		// virtual methods

		// overloaded operators

//-----------------------------------------------------------
}; // end class CSkyBoard

#endif // ifndef _CSkyBoard_h_ 