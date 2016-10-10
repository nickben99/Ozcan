/*--------------------------------------------
CODE BASED ON TUTORIAL AT http://rsn.gamedev.net/tutorials/ms3danim.asp

AND TUTORIAL AT http://hsfortuna.pwp.blueyonder.co.uk
--------------------------------------------*/

//CModelLoader.h, header file for CModelLoader.cpp

//system includes
#include <Rendering/OpenGLInclude.h>
//---------------

#ifndef _CModelLoader_h_
#define _CModelLoader_h_

//header files---
#include "CModel.h"
#include "FileReading/CTextFileReader.h"
//---------------

//defines--------------
//---------------------

//external variables --
//---------------------

//global variables-----
//---------------------

class CModelLoader
{
	private:
//private variables------------

//-----------------------------

//private methods--------------
		// load model textures
		static int reloadTextures(CModel *thisModel);
//-----------------------------
	public:
//public methods---------------
		CModelLoader(); // constructor
		~CModelLoader(); // destructor
		// loads a single model and saves it to the model param
		static int load(char* filename, CModel **model); 
		static void setUpBones(CModel * model);
//-----------------------------

//public variables-------------
//-----------------------------
}; // class CModelLoader

#endif // _CModelLoader_h_