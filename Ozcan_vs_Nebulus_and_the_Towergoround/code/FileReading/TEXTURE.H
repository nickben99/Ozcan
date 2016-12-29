/*------------------------------------------------------------------


// CLASS BASED ON ORIGINAL FROM THE OPENGL SUPERBIBLE


------------------------------------------------------------------*/
/*
 * Texture image definitions from Chapter 8.
 *
 * Written by Michael Sweet
 */

/*
 * Include necessary headers.
 */

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <vector>		// include this to use the vector class

#include "imageFileLoaders.h"

struct textureObject // struct describing each texture object
{
	int currentlyInUse;

	static const int MaxBuffer = 256; 
	char texturePath[MaxBuffer];
	unsigned int ID;

	void AddReference()
	{
		++currentlyInUse;
	}
};

/*
 * Make this header file work with C and C++ source code...
 */

//#  ifdef __cplusplus
//extern "C" {
//#  endif /* __cplusplus */
//
//#  ifdef __cplusplus
//}
//#  endif /* __cplusplus */
//
//// for seamless textures
//#define GL_CLAMP_TO_EDGE 0x812F

const int InvalidTextureID = 0;

textureObject* FindExistingTexture(const char* filename);

/*	returns a texture number zero or greater
	if e.g 0, 1, 2, 3 are created, then another is needed
	it will be given 4. if e.g 0, 1, 2, 3 are created then 2
	is deleted and another is required it will be given 2.
	The reason for this is that calling glGenTexture too many times 
	supposedly causes memory leaks */
int getTextureNumber(const char* filename);

// delete texture (must be an existing texture)
void deleteTexture(int theTexture);

// clear the vector of texture objects
void clearTextureObjectVector( void);

#endif /* !_TEXTURE_H_ */


/*
 * End of "$Id: texture.h,v 1.2 1999/08/31 04:32:51 mike Exp $".
 */





