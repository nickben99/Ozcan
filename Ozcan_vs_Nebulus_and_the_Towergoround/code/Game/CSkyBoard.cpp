//CSkyBoard.cpp, the sky board class

// system includes --------
#include <math.h>
//-------------------------

// header files -----------
#include "CSkyBoard.h"
#include "Game/Globals.h"
//-------------------------

extern GLuint TextureLoad(const char *filename, bool alpha, GLenum minfilter, GLenum magfilter, GLenum wrap);

// constructor
CSkyBoard::CSkyBoard()
{
	theCamera = CCamera::instance(); // get singleton camera instance
	theTower = CTower::instance(); // get singleton tower instance
}

// overloaded constructor
CSkyBoard::CSkyBoard(CModel * aModel)
	: CSprite(aModel) 
	, skyTex(InvalidTextureID)
{
	theCamera = CCamera::instance(); // get singleton camera instance
	theTower = CTower::instance(); // get singleton tower instance
}

// initialise the class
bool CSkyBoard::initialise(char * backgroundTexture)
{
	CSprite::initialise(); // call super class initialise

	// load the sky background texture
	skyTex =	TextureLoad(backgroundTexture, 
				GL_FALSE, GL_LINEAR, GL_LINEAR, GL_REPEAT);

	if (skyTex <= InvalidTextureID){ // if their were problems loading the texture
		return( false);} // unsuccesful load

	// set the dimensions of the sky board 
	/*	towers with wider steps require a skyboard with a larger width as Nebulus will 
		shoot off the step faster when hit and the edges of the skyboard can be seen */
	leftSide			= 240.0f + ((theTower->xPercentFromBaseMeasurement-1.0f)*30.0f); 
	fullLength			= leftSide*2.0f;
	rightSide			= leftSide - fullLength; //-200.0f;
	distToBillboard		= 200.0f;
	top					= 200.0f;
	bottom				= -600.0f;
	// set texture coordinates
	perCentOfTexLeft	= 0.0f;
	perCentOfTexRight	= fullLength/(2.0f*PI*distToBillboard); 
	normalTexDist		= perCentOfTexRight - perCentOfTexLeft;
	texOffset			= 0.0f;

#ifdef USE_SHADERS
	// introScreenBuffer -----------------------------------------------------------------------------------------------------------
	float vertexPositionArray[12];
	vertexPositionArray[0] =	rightSide; vertexPositionArray[1] = top; vertexPositionArray[2] = distToBillboard;
	vertexPositionArray[3] =	leftSide; vertexPositionArray[4] = top; vertexPositionArray[5] = distToBillboard;
	vertexPositionArray[6] =	rightSide; vertexPositionArray[7] = bottom; vertexPositionArray[8] = distToBillboard;
	vertexPositionArray[9] =	leftSide; vertexPositionArray[10] = bottom; vertexPositionArray[11] = distToBillboard;

	meshBuffer.CreateVertexArray(vertexPositionArray, 12);	

	meshBuffer.SetColor(CVector4(1.0f, 1.0f, 1.0f, 1.0f));

	float vertexTexCoordsArray[8];
	memset(vertexTexCoordsArray, 0, sizeof(float)*8); // will be set each frame

	meshBuffer.CreateTexCoordArray(vertexTexCoordsArray, 8, true);
	meshBuffer.SetTexture(skyTex);

	unsigned short vertexIndecisArray[6];
	vertexIndecisArray[0] = 0; vertexIndecisArray[1] = 1; vertexIndecisArray[2] = 2;
	vertexIndecisArray[3] = 3; vertexIndecisArray[4] = 2; vertexIndecisArray[5] = 1;

	meshBuffer.CreateIndexArray(vertexIndecisArray, 6);
#endif

	return( true ); // succesful load
}

/*	move the sky board 
	(must be done after the camera position has been set for the frame) */
void CSkyBoard::move(GLfloat)
{
	// the sky board is always a set distence from the camera on the ground plane
	position = theCamera->GetGameCameraPosition();
	position.y = 5.0f; // the sky board height is constant

	rotation = theCamera->GetGameCameraSkyBoardRotation(); // rotate the sky board so its always in front of the camera

	// calculate the texture offset
	texOffset = -(theCamera->currentYRotation / (2.0f*PI));
	// texture coordinates can't go higher than 1.0f
	if (texOffset > 1.0f){
		texOffset -= 1.0f;}
	// texture coordinates can't go lower than 0.0f
	if (texOffset < 0.0f){
		texOffset += 1.0f;}

	calculateFinalMatrix(); // calculate the sky boards final matrix
}

// draw the sky board
void CSkyBoard::draw()
{
#ifndef USE_SHADERS
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // set polygon mode
	Globals::Instance().gl.GLDisable(GL_BLEND); // disable blending, no transparency
#endif

	// set the colour of the sky board to compleatly white
	float	ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f},
			diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f},
			specular[4] = {1.0f, 1.0f, 1.0f, 1.0f},
			emissive[4] = {1.0f, 1.0f, 1.0f, 1.0f},
			shinnines = 0.0f;

#ifdef USE_SHADERS			
	float vertexTexCoordsArray[] =
	{
		texOffset+perCentOfTexRight, 0.0f,
		texOffset, 0.0f,
		texOffset+perCentOfTexRight, 1.0f,
		texOffset, 1.0f
	};
	meshBuffer.UpdateTexCoordArray(vertexTexCoordsArray);

	Globals::Instance().gl.GLDisable(GL_LIGHTING);	
#endif

	Globals::Instance().gl.GLMaterial(GL_AMBIENT, ambient);
	Globals::Instance().gl.GLMaterial(GL_DIFFUSE, diffuse);
	Globals::Instance().gl.GLMaterial(GL_SPECULAR, specular);
	Globals::Instance().gl.GLMaterial(GL_EMISSION, emissive);
	Globals::Instance().gl.GLMaterial(GL_SHININESS, shinnines);

#ifndef USE_SHADERS
	Globals::Instance().gl.GLEnable(GL_TEXTURE_2D); // enable texturing
	glBindTexture(GL_TEXTURE_2D, skyTex); // set the texture to be used
#endif

	Globals::Instance().modelMatrixStack.PushMatrix();
	Globals::Instance().modelMatrixStack.MultMatrix(finalMatrix);
#ifdef USE_SHADERS	
	meshBuffer.Draw();
	Globals::Instance().gl.ShowSpecularHighlights(true);
#else
	glBegin( GL_TRIANGLES ); // draw the sky board
		// triangle one
		glTexCoord2f(texOffset+perCentOfTexRight, 0.0f);	glVertex3f(rightSide, top, distToBillboard);
		glTexCoord2f(texOffset, 0.0f);						glVertex3f(leftSide, top, distToBillboard);
		glTexCoord2f(texOffset+perCentOfTexRight, 1.0f);	glVertex3f(rightSide, bottom, distToBillboard);
		// triangle two
		glTexCoord2f(texOffset, 1.0f);						glVertex3f(leftSide, bottom, distToBillboard);
		glTexCoord2f(texOffset+perCentOfTexRight, 1.0f);	glVertex3f(rightSide, bottom, distToBillboard);
		glTexCoord2f(texOffset, 0.0f);						glVertex3f(leftSide, top, distToBillboard);
	glEnd();	
#endif	
	Globals::Instance().modelMatrixStack.PopMatrix();

	Globals::Instance().gl.GLEnable(GL_LIGHTING);
}

// destructor
CSkyBoard::~CSkyBoard()
{
	CSprite::~CSprite(); // delete super class
	deleteSkyBoard();
}

// delete sky board
void CSkyBoard::deleteSkyBoard()
{
	deleteTexture(skyTex); // free the openGL memory
#ifdef USE_SHADERS
	meshBuffer.Delete();
#endif
}