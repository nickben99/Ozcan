//------------------------------------------------------------------


// CLASS BASED ON ORIGINAL FROM THE OPENGL SUPERBIBLE


//------------------------------------------------------------------
/*
 * Texture image routines from Chapter 8.
 *
 * Written by Michael Sweet
 */

/*
 * Include necessary headers.
 */

#include "texture.h"
#include <Game/defines.h>
#include "assert.h"
#include <System/Interfaces/SystemInterface.h>
#include <Rendering/OpenGLInclude.h>

#ifdef USE_SHADERS
#include <Rendering/OpenGLShader.h>
#else
#define CHECK_GL_ERROR
#endif

using namespace std;	// set the namespace to standard

// hold the texture objects, whether they are being used and their indices
vector<textureObject> textureObjects;

/*
 * 'TextureLoad()' - Load a bitmap file and define it as a 1D or 2D texture.
 *
 * Returns a value greater than zero on success and zero or less on error.
 */

GLuint                           /* O - Texture object or 0 on error */
TextureLoad(const char      *filename, /* I - Bitmap file to load */
            bool,     /* I - Generate alpha for bitmap */
            GLenum    minfilter, /* I - Minification filter */
	    GLenum    magfilter,	/* I - Magnification filter */
	    GLenum    wrap)			/* I - Repeat or clamp */
{
	textureObject* existingTexture = FindExistingTexture(filename);
	if (existingTexture)
	{
		existingTexture->AddReference();
		return existingTexture->ID;
	}

	GLubyte	*bits = 0;					/* Bitmap RGB pixels */
	GLuint      texture = 0,				/* Texture object */
				uPixelFormat = GL_RGB;	// GL_RGBA / GL_RGB
	unsigned short	uHeight = 256,		// height of the image
						uWidth = 256;			// width of the image
	unsigned char		uBytesPerPixel = 4;

	// determine what type of image this is based on the file extension
	if ( strstr( filename, ".bmp" ) ) /*	if strstr doesn't return null, a ".bmp" 
											file extension has been found */
	{
		// Try loading the bitmap file...
		bits = LoadDIBitmap( filename, uHeight, uWidth, uBytesPerPixel);
		CHECK_GL_ERROR;
		if (bits == (GLubyte *)0){
			return(0);} // loading failed
	}

	if ( strstr( filename, ".tga" ) ) /* if strstr doesn't return null, a ".tga" 
										file extension has been found */
	{
		// try loading the targa file
		bits = LoadTarga( filename, uHeight, uWidth, uBytesPerPixel);
		CHECK_GL_ERROR;
		if (bits == (GLubyte *)0){
			return(0);} // loading failed
	}

	// if bits is still null the file was neither a targa or a bitmap
	if ( !bits ){
        char errorMsg[256];
		SPRINTF( errorMsg, "%s is an unsupported file type", filename );
        ErrorOutput(errorMsg);
		return( 0 );} // not a supported file type

	// figure out the pixel format
	if ( uBytesPerPixel == 4 ){
		uPixelFormat = GL_RGBA;}

    /*	use getTextureNumber() to generate an unsigned int then 
		glBindTexture will bring the texture into scope and glTexImage2D 
		will allocate the bitmap to the texture */
	//texture = getTextureNumber(filename);
    glGenTextures(1, &texture);
	CHECK_GL_ERROR;
    
	glBindTexture(GL_TEXTURE_2D, texture);

	CHECK_GL_ERROR;

    /* Set texture parameters */
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
//    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    CHECK_GL_ERROR;
    /*
     * Set texture image; if the minification filter uses mip-mapping
     * then use gluBuild2D/1DMipmaps() to load the texture...
     */

//    if (minfilter == GL_LINEAR || minfilter == GL_NEAREST)
//	{
        glTexImage2D( GL_TEXTURE_2D, 0, uPixelFormat, uWidth, uHeight, 0, uPixelFormat, GL_UNSIGNED_BYTE, bits);
		CHECK_GL_ERROR;
//	}
//    else // mipmaps required
//	{
//        gluBuild2DMipmaps( GL_TEXTURE_2D, uPixelFormat, uWidth, uHeight, uPixelFormat, GL_UNSIGNED_BYTE, bits);
//		CHECK_GL_ERROR;
//	}

    // Free the bitmap and return... 
    free(bits);
	bits = 0;
//#endif
    return (texture);
   }

textureObject* FindExistingTexture(const char* filename)
{
	int totalTextures = (int)textureObjects.size();
	
	// go through vector
	for (int tex = 0; tex < totalTextures; tex++)
	{
		// referance variable (speed and readability)
		textureObject &texture = textureObjects[tex];
		if (0 == strcmp(filename, texture.texturePath))
		{
			return &texture;
		}
	}
	return NULL;
}

/*	returns a texture number one or greater
	if e.g 1, 2, 3 are created, then another is needed
	it will be given 4. if e.g 1, 2, 3 are created then 2
	is deleted and another is required it will be given 2.
	The reason for this is that calling glGenTexture too many times 
	supposedly causes memory leaks */
int getTextureNumber(const char* filename)
{
	// get the amount of textures
	int totalTextures = (int)textureObjects.size();
	
	// go through vector and find the first free slot
	for (int tex = 0; tex < totalTextures; tex++)
	{
		// referance variable (speed and readability)
		textureObject &texture = textureObjects[tex];
		if (texture.currentlyInUse == 0)
		{
			texture.currentlyInUse = 1; // the texture object is now in use
            memset(texture.texturePath, '\0', sizeof(char)*textureObject::MaxBuffer);
            defines::strncpy(texture.texturePath, textureObject::MaxBuffer-1, filename, (int)strlen(filename));
			return((int)texture.ID); // free slot found
		}
	}
	// no free texture available, so must create a new one
	textureObject texObject;
	textureObjects.push_back(texObject);

	textureObject &texture = textureObjects[totalTextures]; // referance variable
	// set-up the texture object
	texture.currentlyInUse = 1; // now in use
    memset(texture.texturePath, '\0', sizeof(char)*textureObject::MaxBuffer);
    defines::strncpy(texture.texturePath, textureObject::MaxBuffer-1, filename, (int)strlen(filename));
	/*	assign ID as +1 e.g element 0 will have ID 1, 1 ID 2 and so on as 
		texture objects start at 1 */
	texture.ID = (totalTextures+1); 

	return( texture.ID );
}

// delete texture
void deleteTexture(int theTexture)
{
	const GLuint textureToDelete = theTexture;

	// get the amount of textures
	unsigned int totalTextures = (int)textureObjects.size();

	if( (textureToDelete <= InvalidTextureID) || 
		(textureToDelete > (totalTextures))){
		return;} // out of bounds

	textureObject &textureObj = textureObjects[(textureToDelete-1)];
	// check the texture is in use
	if (textureObj.currentlyInUse > 0)
	{
		--textureObj.currentlyInUse;
		if (0 == textureObj.currentlyInUse)
		{
			glDeleteTextures(1, &textureToDelete); // delete the texture (textureToDelete must be constant unsigned int pointer)
			textureObj.texturePath[0] = '\0';
		}
	}
}

// clear the texture vector
void clearTextureObjectVector( void)
{
	textureObjects.clear();
}