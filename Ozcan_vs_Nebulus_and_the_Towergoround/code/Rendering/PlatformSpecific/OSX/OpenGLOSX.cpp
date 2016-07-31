// OpenGLOSX.cpp - keymap implementation class

#ifdef OSX

//--- System includes ---------------
#include <Rendering/OpenGLInclude.h>

#include <iostream>
#include <string>
//----------------------------------

//--- Header files ---------
#include "OpenGLOSX.h" // the header file for this class
#include "Game/Globals.h"
#ifdef OSX
#include <Math/CVector.h>
#endif
//--------------------------

const int InvalidLocation = -1;

OpenGLImplementation::OpenGLImplementation()
#ifdef _WINDOWS
#ifdef USE_SHADERS
	: useTextureSubFunctionLocation(0)
	, noTextureSubFunctionLocation(0)

	, textureSamplerLocation(InvalidLocation)
	
	, useLightingSubFunctionLocation(0)
	, noLightingSubFunctionLocation(0)

	, projectionMatrixLocation(InvalidLocation)
	, modelMatrixLocation(InvalidLocation)
	, viewMatrixLocation(InvalidLocation)
	, vertexPositionAttribLocation(InvalidLocation)
	, vertexNormalAttribLocation(InvalidLocation)
	, vertexTexCoordAttribLocation(InvalidLocation)
	, ambientColorLocation(InvalidLocation)
	, diffuseColorLocation(InvalidLocation)
	, specularColorLocation(InvalidLocation)
	, emissiveColorLocation(InvalidLocation)
	, shininessLocation(InvalidLocation)
	, showSpecularHighlightLocation(InvalidLocation)
	, hasBeenInitialized(false)

	, textureSubRoutineUniform(0)
	, colorSubRoutineUniform(0)
	, lightingSubRoutineUniform(0)
#endif
#endif
{

}

OpenGLImplementation::~OpenGLImplementation()
{
}

#ifdef _WINDOWS
#ifdef USE_SHADERS

bool OpenGLImplementation::InitGL()									
{
	hasBeenInitialized = true;
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	CHECK_GL_ERROR;

    if (GLEW_OK != err)
    {		
        return false;
    }

#if _DEBUG && _WINDOWS
	const GLubyte* version = glGetString(GL_SHADING_LANGUAGE_VERSION);
	OutputDebugString((char*)version);
#endif

	if (!shader.CreateProgram("shader.vs", "shader.fs"))
	{
		return false;
	}
	
	shader.UseProgram();
	shader.CacheSubroutineUniforms();

	textureSamplerLocation = GetUniformLocation("uSampler");
	
	projectionMatrixLocation = GetUniformLocation("uProjectionMatrix");
	modelMatrixLocation = GetUniformLocation("uModelMatrix");
	viewMatrixLocation =  GetUniformLocation("uViewMatrix");

	ambientColorLocation = GetUniformLocation("uAmbientColor");
	diffuseColorLocation = GetUniformLocation("uPointLightingDiffuseColor");
	specularColorLocation = GetUniformLocation("uPointLightingSpecularColor");
	emissiveColorLocation = GetUniformLocation("uPointLightingEmissiveColor");
	shininessLocation = GetUniformLocation("uMaterialShininess");
	showSpecularHighlightLocation = GetUniformLocation("uShowSpecularHighlights");

	vertexPositionAttribLocation = GetAttribLocation("aVertexPosition");
	vertexNormalAttribLocation = GetAttribLocation("aVertexNormal");
	vertexTexCoordAttribLocation = GetAttribLocation("aTextureCoord");

	textureSubRoutineUniform = GetSubroutineUniformIndex("textureRender", GL_FRAGMENT_SHADER);
	colorSubRoutineUniform = GetSubroutineUniformIndex("colorRender", GL_FRAGMENT_SHADER);
	lightingSubRoutineUniform = GetSubroutineUniformIndex("lightingRender", GL_FRAGMENT_SHADER);

	GLint o = 0;
	glGetProgramStageiv(shader.GetShaderProgram(), GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &o);
	CHECK_GL_ERROR;

	useTextureSubFunctionLocation = GetSubroutineIndex("RenderWithTexture", GL_FRAGMENT_SHADER);
	noTextureSubFunctionLocation = GetSubroutineIndex("RenderNOTexture", GL_FRAGMENT_SHADER);
	noLightingSubFunctionLocation = GetSubroutineIndex("RenderNoLighting", GL_FRAGMENT_SHADER);

#ifdef _DEBUG
	useLightingSubFunctionLocation = GetSubroutineIndex("CalculateSceneLighting_DebugBuild", GL_FRAGMENT_SHADER);
	shader.SetSubroutineUniformIndex(colorSubRoutineUniform, GetSubroutineIndex("CalculateFragmentColor_DebugBuild", GL_FRAGMENT_SHADER), GL_FRAGMENT_SHADER, false, false);
#else
	useLightingSubFunctionLocation = GetSubroutineIndex("CalculateSceneLighting_ReleaseBuild", GL_FRAGMENT_SHADER);
	shader.SetSubroutineUniformIndex(colorSubRoutineUniform, GetSubroutineIndex("CalculateFragmentColor_ReleaseBuild", GL_FRAGMENT_SHADER), GL_FRAGMENT_SHADER, false, false);
#endif

	shader.SetSubroutineUniformIndex(textureSubRoutineUniform, noTextureSubFunctionLocation, GL_FRAGMENT_SHADER, false, false);
	shader.SetSubroutineUniformIndex(lightingSubRoutineUniform, useLightingSubFunctionLocation, GL_FRAGMENT_SHADER, true, true); // <== last one has true for forceFlush
	return true;
}

void OpenGLImplementation::DestroyGL()
{
	shader.DestroyProgram();
}

bool OpenGLImplementation::HasBeenInitialized()
{
	return hasBeenInitialized;
}

void OpenGLImplementation::UseProgram()
{
	shader.UseProgram();
}

void OpenGLImplementation::StopUsingProgram()
{
	glUseProgram(0);
	CHECK_GL_ERROR;
}

int OpenGLImplementation::GetShaderProgram()
{
	return shader.GetShaderProgram();
}

int OpenGLImplementation::GetUniformLocation(const char* pUniformVariableName)
{
	return shader.GetUniformLocation(pUniformVariableName);
}

unsigned int OpenGLImplementation::GetSubroutineUniformIndex(const char* pSubroutineUniformVariableName, unsigned int shaderType)
{
	return shader.GetSubroutineUniformIndex(pSubroutineUniformVariableName, shaderType);
}

void OpenGLImplementation::SetSubroutineUniformIndex(int subRoutineUniformIndex, unsigned int subRoutineIndex, unsigned int shaderType)
{
	shader.SetSubroutineUniformIndex(subRoutineUniformIndex, subRoutineIndex, shaderType, true, false);
}

int OpenGLImplementation::GetSubroutineIndex(const char* pSubRoutineName, unsigned int shaderType)
{
	return shader.GetSubroutineIndex(pSubRoutineName, shaderType);
}

int OpenGLImplementation::GetAttribLocation(const char* pUniformVariableName)
{
	return shader.GetAttribLocation(pUniformVariableName);
}

void OpenGLImplementation::SetUniformFloat(int location, float newValue)
{
	shader.SetUniformFloat(location, newValue);
}

void OpenGLImplementation::SetUniformBool(int location, bool newValue)
{
	shader.SetUniformBool(location, newValue);
}

void OpenGLImplementation::SetUniformInt(int location, int newValue)
{
	shader.SetUniformInt(location, newValue);
}

void OpenGLImplementation::SetUniformMatrix(int location, const CMatrix& mat)
{
	shader.SetUniformMatrix(location, mat);
}

void OpenGLImplementation::SetUniformVector3(int location, const CVector& vect)
{
	shader.SetUniformVector3(location, vect);
}

void OpenGLImplementation::SetUniformVector4(int location, const CVector4& vect)
{
	shader.SetUniformVector4(location, vect);
}

void OpenGLImplementation::UseTextureDefault(int textureIndex)
{
	UseTexture(0, textureIndex, textureSamplerLocation);
	SetSubroutineUniformIndex(textureSubRoutineUniform, useTextureSubFunctionLocation, GL_FRAGMENT_SHADER);
}

void OpenGLImplementation::UseTexture(int textureBindLayer, int textureIndex, int textureSamplerLoc)
{
	const int textureBindConstants[] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6,
										GL_TEXTURE7,GL_TEXTURE8, GL_TEXTURE9, GL_TEXTURE10, GL_TEXTURE11,GL_TEXTURE12,GL_TEXTURE13,
										GL_TEXTURE14, GL_TEXTURE15, GL_TEXTURE16, GL_TEXTURE17, GL_TEXTURE18, GL_TEXTURE19, GL_TEXTURE20,
										GL_TEXTURE21, GL_TEXTURE22, GL_TEXTURE23, GL_TEXTURE24, GL_TEXTURE25, GL_TEXTURE26, GL_TEXTURE27,
										GL_TEXTURE28, GL_TEXTURE29, GL_TEXTURE30};

	glActiveTexture(textureBindConstants[textureBindLayer]);
	CHECK_GL_ERROR;
	glBindTexture(GL_TEXTURE_2D, textureIndex); // bind texture required
	CHECK_GL_ERROR;
	SetUniformInt(textureSamplerLoc, textureBindLayer);
}

void OpenGLImplementation::DontUseDefaultTexture()
{
	SetSubroutineUniformIndex(textureSubRoutineUniform, noTextureSubFunctionLocation, GL_FRAGMENT_SHADER);
}

void OpenGLImplementation::DeleteVertexBufferObject(unsigned int vertexBufferObject)
{
	glDeleteBuffers( 1, &vertexBufferObject);
	CHECK_GL_ERROR;
}

void OpenGLImplementation::BindBuffer(unsigned int bufferType, int vertexBufferObject)
{
	glBindBuffer(bufferType, vertexBufferObject);
	CHECK_GL_ERROR;
}

void OpenGLImplementation::VertexAttribPointer(int location, int numComponents, int stride, int offset)
{
	glEnableVertexAttribArray(location);
	CHECK_GL_ERROR;
	const GLvoid* offsetPointer = (const GLvoid*)(long)offset;
	glVertexAttribPointer(location, numComponents, GL_FLOAT, GL_FALSE, stride, offsetPointer);
	CHECK_GL_ERROR;
}

void OpenGLImplementation::DisableVertexPositionAttribPointer()
{
	glDisableVertexAttribArray(vertexPositionAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementation::SetVertexPositionAttribPointer(int stride, int offset)
{
	const int NumComponents = 3;
	VertexAttribPointer(vertexPositionAttribLocation, NumComponents, stride, offset);
}

void OpenGLImplementation::DisableVertexNormalAttribPointer()
{
	glDisableVertexAttribArray(vertexNormalAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementation::SetVertexNormalAttribPointer(int stride, int offset)
{
	const int NumComponents = 3;
	VertexAttribPointer(vertexNormalAttribLocation, NumComponents, stride, offset);
}

void OpenGLImplementation::DisableVertexTextureAttribPointer()
{
	glDisableVertexAttribArray(vertexTexCoordAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementation::SetVertexTextureAttribPointer(int stride)
{
	const int NumComponents = 2;
	VertexAttribPointer(vertexTexCoordAttribLocation, NumComponents, stride);
}

void OpenGLImplementation::DrawElements(int numToDraw)
{
	glDrawElements( GL_TRIANGLES, numToDraw, GL_UNSIGNED_SHORT, 0);
	CHECK_GL_ERROR;	
}

void OpenGLImplementation::DrawArrays(int numToDraw)
{
	glDrawArrays(GL_TRIANGLES, 0, numToDraw);
	CHECK_GL_ERROR;
}

void OpenGLImplementation::SetProjectionMatrix(const CMatrix& mat)
{
	SetUniformMatrix(projectionMatrixLocation, mat);
}

void OpenGLImplementation::SetModelMatrix(const CMatrix& mat)
{
	SetUniformMatrix(modelMatrixLocation, mat);
}

void OpenGLImplementation::SetViewMatrix(const CMatrix& mat)
{
	SetUniformMatrix(viewMatrixLocation, mat);
}

void OpenGLImplementation::SetAmbientColor(const CVector& color)
{
	SetUniformVector3(ambientColorLocation, color);
}

void OpenGLImplementation::SetDiffuseColor(const CVector& color)
{
	SetUniformVector3(diffuseColorLocation, color);
}

void OpenGLImplementation::SetSpecularColor(const CVector& color)
{
	SetUniformVector3(specularColorLocation, color);
}

void OpenGLImplementation::SetEmissiveColor(const CVector4& color)
{
	SetUniformVector4(emissiveColorLocation, color);
}

void OpenGLImplementation::SetShininess(float val)
{
	SetUniformFloat(shininessLocation, val);
}

void OpenGLImplementation::ShowSpecularHighlights(bool show)
{
	SetUniformBool(showSpecularHighlightLocation, show);
}

#else
// lighting and rendering context set-up
bool OpenGLImplementation::InitGL()									
{
	// Light values and coordinates
	GLfloat  ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat  diffuseLight[] = {1.2f, 1.2f, 1.2f, 1.0f};

	// clear window to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f );

	glEnable(GL_DEPTH_TEST);	// hidden surface removal
	glFrontFace(GL_CCW);		// counter-clockwise wound poygons can be seen
	// back face culling (don't draw backfaces)
	glEnable(GL_CULL_FACE); 

	// enable lighting
	glEnable(GL_LIGHTING);

	// setup light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE, diffuseLight);
	// turn on the light
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH); // guroud shading
	glClearDepth(1.0f);	// depth buffer setup
	glDepthFunc(GL_LEQUAL); // the type of depth testing to do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// perspective calculations

	return true;
}
#endif

//called each time the scene is resized (and on initialisation)
void OpenGLImplementation::ReSizeGLScene(int width, int height)		
{
	if (height==0){ // prevent a divide by zero
		height=1;}

	Globals::Instance().windowWidth	= (float)width;
	Globals::Instance().windowHeight = (float)height; // keep a record of the window width and height

	glViewport(0, 0, width, height); // reset the current viewport
}

void OpenGLImplementation::GLEnable(unsigned int identifier)
{
#ifdef USE_SHADERS
	switch (identifier)
	{
		case GL_LIGHTING:
			SetSubroutineUniformIndex(lightingSubRoutineUniform, useLightingSubFunctionLocation, GL_FRAGMENT_SHADER);
			break;
		default: 
			glEnable(identifier);
			break;
	}
#else
	glEnable(identifier);
#endif
}

void OpenGLImplementation::GLDisable(unsigned int identifier)
{
#ifdef USE_SHADERS
	switch (identifier)
	{
		case GL_LIGHTING:
			SetSubroutineUniformIndex(lightingSubRoutineUniform, noLightingSubFunctionLocation, GL_FRAGMENT_SHADER);
			break;
		default: 
			glDisable(identifier);
			break;
	}
#else
	glDisable(identifier);
#endif
}

void OpenGLImplementation::GLMaterial(unsigned int identifier, float value)
{
#ifdef USE_SHADERS
	switch (identifier)
	{
		case GL_SHININESS:
			SetShininess(value);
			break;
		default: 
			glMaterialf( GL_FRONT, identifier, value);
			break;
	}
#else
	glMaterialf( GL_FRONT, identifier, value);
#endif
}

void OpenGLImplementation::GLMaterial(unsigned int identifier, float* valueArrayPointer)
{
#ifdef USE_SHADERS
	switch (identifier)
	{
		case GL_AMBIENT:
			SetAmbientColor(CVector(valueArrayPointer[0], valueArrayPointer[1], valueArrayPointer[2]));
			break;
		case GL_DIFFUSE:
			SetDiffuseColor(CVector(valueArrayPointer[0], valueArrayPointer[1], valueArrayPointer[2]));
			break;
		case GL_SPECULAR:
			SetSpecularColor(CVector(valueArrayPointer[0], valueArrayPointer[1], valueArrayPointer[2]));
			break;
		case GL_EMISSION:
			SetEmissiveColor(CVector4(valueArrayPointer[0], valueArrayPointer[1], valueArrayPointer[2], valueArrayPointer[3]));
			break;
		default: 
			glMaterialfv( GL_FRONT, identifier, valueArrayPointer);
			break;
	}
#else
	glMaterialfv( GL_FRONT, identifier, valueArrayPointer);
#endif
}

#endif // _WINDOWS

#endif