// OpenGLBase.cpp

//--- System includes ---------------
#ifdef _WINDOWS
#include <glew-1.12.0\include\GL/glew.h>
#include <windows.h>	// header file for windows
#include <gl\glu.h>		// header file for the GLu32 library
#endif

#ifdef OSX
#include </usr/local/Cellar/glew/1.13.0/include/GL/glew.h>
#endif

#include <iostream>
#include <string>
//----------------------------------

//--- Header files ---------
#include "OpenGLImplementationBase.h" // the header file for this class
#include "Game/Globals.h"
#ifdef OSX
#include <Math/CVector.h>
#endif
//--------------------------

const int InvalidLocation = -1;

OpenGLImplementationBase::OpenGLImplementationBase()
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
    , oldCodeTextureSelection(0)
	, colorSubRoutineUniform(0)
	, lightingSubRoutineUniform(0)
    , oldCodeLightingSelection(0)
    , isUsingSubRoutines(true)
#endif
{

}

OpenGLImplementationBase::~OpenGLImplementationBase()
{
}

#ifdef USE_SHADERS

bool OpenGLImplementationBase::IsUsingSubRoutines()
{
    return isUsingSubRoutines;
}

bool OpenGLImplementationBase::InitGL()									
{
	hasBeenInitialized = true;
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	CHECK_GL_ERROR;

    if (GLEW_OK != err)
    {		
        return false;
    }

#ifdef _DEBUG
	const char* version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
#ifdef _WINDOWS
    OutputDebugString("\n");
    OutputDebugString(version);
#elif OSX
    std::cout << "\n" << version;
#endif
#endif

    isUsingSubRoutines = true;
    if (!shader.CreateProgram("shader.vs", "shader.fs"))
    {
        shader.DestroyProgram();
        isUsingSubRoutines = false;
        if (!shader.CreateProgram("shaderOld.vs", "shaderOld.fs"))
        {
            return false;
        }
    }
	
	shader.UseProgram();
    if (isUsingSubRoutines) {
        shader.CacheSubroutineUniforms();
    }

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

    if (isUsingSubRoutines) {
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
    }
    else
    {
        oldCodeTextureSelection = GetUniformLocation("uTextureRender");
        SetUniformBool(oldCodeTextureSelection, false);
        
        oldCodeLightingSelection = GetUniformLocation("uLightingRender");
        SetUniformBool(oldCodeLightingSelection, true);
    }
	return true;
}

void OpenGLImplementationBase::DestroyGL()
{
	shader.DestroyProgram();
}

bool OpenGLImplementationBase::HasBeenInitialized()
{
	return hasBeenInitialized;
}

void OpenGLImplementationBase::UseProgram()
{
	shader.UseProgram();
}

void OpenGLImplementationBase::StopUsingProgram()
{
	glUseProgram(0);
	CHECK_GL_ERROR;
}

int OpenGLImplementationBase::GetShaderProgram()
{
	return shader.GetShaderProgram();
}

int OpenGLImplementationBase::GetUniformLocation(const char* pUniformVariableName)
{
	return shader.GetUniformLocation(pUniformVariableName);
}

unsigned int OpenGLImplementationBase::GetSubroutineUniformIndex(const char* pSubroutineUniformVariableName, unsigned int shaderType)
{
	return shader.GetSubroutineUniformIndex(pSubroutineUniformVariableName, shaderType);
}

void OpenGLImplementationBase::SetSubroutineUniformIndex(int subRoutineUniformIndex, unsigned int subRoutineIndex, unsigned int shaderType)
{
	shader.SetSubroutineUniformIndex(subRoutineUniformIndex, subRoutineIndex, shaderType, true, false);
}

int OpenGLImplementationBase::GetSubroutineIndex(const char* pSubRoutineName, unsigned int shaderType)
{
	return shader.GetSubroutineIndex(pSubRoutineName, shaderType);
}

int OpenGLImplementationBase::GetAttribLocation(const char* pUniformVariableName)
{
	return shader.GetAttribLocation(pUniformVariableName);
}

void OpenGLImplementationBase::SetUniformFloat(int location, float newValue)
{
	shader.SetUniformFloat(location, newValue);
}

void OpenGLImplementationBase::SetUniformBool(int location, bool newValue)
{
	shader.SetUniformBool(location, newValue);
}

void OpenGLImplementationBase::SetUniformInt(int location, int newValue)
{
	shader.SetUniformInt(location, newValue);
}

void OpenGLImplementationBase::SetUniformMatrix(int location, const CMatrix& mat)
{
	shader.SetUniformMatrix(location, mat);
}

void OpenGLImplementationBase::SetUniformVector3(int location, const CVector& vect)
{
	shader.SetUniformVector3(location, vect);
}

void OpenGLImplementationBase::SetUniformVector4(int location, const CVector4& vect)
{
	shader.SetUniformVector4(location, vect);
}

void OpenGLImplementationBase::UseTextureDefault(int textureIndex)
{
	UseTexture(0, textureIndex, textureSamplerLocation);
    if (isUsingSubRoutines) {
        SetSubroutineUniformIndex(textureSubRoutineUniform, useTextureSubFunctionLocation, GL_FRAGMENT_SHADER);
    }
    else
    {
        SetUniformBool(oldCodeTextureSelection, true);
    }
}

void OpenGLImplementationBase::UseTexture(int textureBindLayer, int textureIndex, int textureSamplerLoc)
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

void OpenGLImplementationBase::DontUseDefaultTexture()
{
    if (isUsingSubRoutines) {
        SetSubroutineUniformIndex(textureSubRoutineUniform, noTextureSubFunctionLocation, GL_FRAGMENT_SHADER);
    }
    else
    {
        SetUniformBool(oldCodeTextureSelection, false);
    }
}

void OpenGLImplementationBase::DeleteVertexBufferObject(unsigned int vertexBufferObject)
{
	glDeleteBuffers( 1, &vertexBufferObject);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::BindBuffer(unsigned int bufferType, int vertexBufferObject)
{
	glBindBuffer(bufferType, vertexBufferObject);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::VertexAttribPointer(int location, int numComponents, int stride, int offset)
{
	glEnableVertexAttribArray(location);
	CHECK_GL_ERROR;
	const GLvoid* offsetPointer = (const GLvoid*)(long)offset;
	glVertexAttribPointer(location, numComponents, GL_FLOAT, GL_FALSE, stride, offsetPointer);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::DisableVertexPositionAttribPointer()
{
	glDisableVertexAttribArray(vertexPositionAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::SetVertexPositionAttribPointer(int stride, int offset)
{
	const int NumComponents = 3;
	VertexAttribPointer(vertexPositionAttribLocation, NumComponents, stride, offset);
}

void OpenGLImplementationBase::DisableVertexNormalAttribPointer()
{
	glDisableVertexAttribArray(vertexNormalAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::SetVertexNormalAttribPointer(int stride, int offset)
{
	const int NumComponents = 3;
	VertexAttribPointer(vertexNormalAttribLocation, NumComponents, stride, offset);
}

void OpenGLImplementationBase::DisableVertexTextureAttribPointer()
{
	glDisableVertexAttribArray(vertexTexCoordAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::SetVertexTextureAttribPointer(int stride)
{
	const int NumComponents = 2;
	VertexAttribPointer(vertexTexCoordAttribLocation, NumComponents, stride);
}

void OpenGLImplementationBase::DrawElements(int numToDraw)
{
	glDrawElements( GL_TRIANGLES, numToDraw, GL_UNSIGNED_SHORT, 0);
	CHECK_GL_ERROR;	
}

void OpenGLImplementationBase::DrawArrays(int numToDraw)
{
	glDrawArrays(GL_TRIANGLES, 0, numToDraw);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::SetProjectionMatrix(const CMatrix& mat)
{
	SetUniformMatrix(projectionMatrixLocation, mat);
}

void OpenGLImplementationBase::SetModelMatrix(const CMatrix& mat)
{
	SetUniformMatrix(modelMatrixLocation, mat);
}

void OpenGLImplementationBase::SetViewMatrix(const CMatrix& mat)
{
	SetUniformMatrix(viewMatrixLocation, mat);
}

void OpenGLImplementationBase::SetAmbientColor(const CVector& color)
{
	SetUniformVector3(ambientColorLocation, color);
}

void OpenGLImplementationBase::SetDiffuseColor(const CVector& color)
{
	SetUniformVector3(diffuseColorLocation, color);
}

void OpenGLImplementationBase::SetSpecularColor(const CVector& color)
{
	SetUniformVector3(specularColorLocation, color);
}

void OpenGLImplementationBase::SetEmissiveColor(const CVector4& color)
{
	SetUniformVector4(emissiveColorLocation, color);
}

void OpenGLImplementationBase::SetShininess(float val)
{
	SetUniformFloat(shininessLocation, val);
}

void OpenGLImplementationBase::ShowSpecularHighlights(bool show)
{
	SetUniformBool(showSpecularHighlightLocation, show);
}

#else
// lighting and rendering context set-up
bool OpenGLImplementationBase::InitGL()									
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
void OpenGLImplementationBase::ReSizeGLScene(int width, int height)		
{
	if (height==0){ // prevent a divide by zero
		height=1;}

	Globals::Instance().windowWidth	= (float)width;
	Globals::Instance().windowHeight = (float)height; // keep a record of the window width and height

#ifdef USE_SHADERS
	//glViewport(-width, 0, width*2, height*2);
	glViewport(0, 0, width, height); // reset the current viewport
#else
	glViewport(0, 0, width, height); // reset the current viewport

	// select the projection matrix for manipulation from this point forwards
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); // reset the projection matrix

	// calculate the aspect ratio of the window
	gluPerspective(45.0f, Globals::Instance().windowWidth/Globals::Instance().windowHeight, 0.1f, 500.0f);
	
	/*	select the modelview matrix for manipulation from this point forwards
		(and subsequently throughout programme) */	
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity(); // reset the modelview matrix
#endif
}

void OpenGLImplementationBase::GLEnable(unsigned int identifier)
{
#ifdef USE_SHADERS
	switch (identifier)
	{
		case GL_LIGHTING:
            if (isUsingSubRoutines) {
                SetSubroutineUniformIndex(lightingSubRoutineUniform, useLightingSubFunctionLocation, GL_FRAGMENT_SHADER);
            }
            else
            {
                SetUniformBool(oldCodeLightingSelection, true);
            }
			break;
		default: 
			glEnable(identifier);
			break;
	}
#else
	glEnable(identifier);
#endif
}

void OpenGLImplementationBase::GLDisable(unsigned int identifier)
{
#ifdef USE_SHADERS
	switch (identifier)
	{
		case GL_LIGHTING:
            if (isUsingSubRoutines) {
                SetSubroutineUniformIndex(lightingSubRoutineUniform, noLightingSubFunctionLocation, GL_FRAGMENT_SHADER);
            }
            else
            {
                SetUniformBool(oldCodeLightingSelection, false);
            }
            
			break;
		default:
			glDisable(identifier);
			break;
	}
#else
	glDisable(identifier);
#endif
}

void OpenGLImplementationBase::GLMaterial(unsigned int identifier, float value)
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

void OpenGLImplementationBase::GLMaterial(unsigned int identifier, float* valueArrayPointer)
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