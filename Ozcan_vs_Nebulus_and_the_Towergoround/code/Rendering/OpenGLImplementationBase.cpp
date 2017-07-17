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
	: //useTextureSubFunctionLocation(0)
    // noTextureSubFunctionLocation(0)
    //textureSamplerLocation(InvalidLocation)
	
	//useLightingSubFunctionLocation(0)
	//noLightingSubFunctionLocation(0)

	//projectionMatrixLocation(InvalidLocation)
	//modelMatrixLocation(InvalidLocation)
	// viewMatrixLocation(InvalidLocation)
	//vertexPositionAttribLocation(InvalidLocation)
	//, vertexNormalAttribLocation(InvalidLocation)
	//, vertexTexCoordAttribLocation(InvalidLocation)
	//ambientColorLocation(InvalidLocation)
	//diffuseColorLocation(InvalidLocation)
	//specularColorLocation(InvalidLocation)
	// emissiveColorLocation(InvalidLocation)
	// shininessLocation(InvalidLocation)
	//showSpecularHighlightLocation(InvalidLocation)
//	, hasBeenInitialized(false)

	//textureSubRoutineUniform(0)
    //, oldCodeTextureSelection(0)
	//, colorSubRoutineUniform(0)
	//, lightingSubRoutineUniform(0)
    //, oldCodeLightingSelection(0)
    isUsingSubRoutines(true)
#endif
{
    renderingShaderParams.useTextureSubFunctionLocation = 0;
    renderingShaderParams.noTextureSubFunctionLocation = 0;
    renderingShaderParams.textureSamplerLocation = InvalidLocation;
    renderingShaderParams.useLightingSubFunctionLocation = 0;
    renderingShaderParams.noLightingSubFunctionLocation = 0;
    renderingShaderParams.projectionMatrixLocation = InvalidLocation;
    renderingShaderParams.modelMatrixLocation = InvalidLocation;
    renderingShaderParams.viewMatrixLocation = InvalidLocation;
    renderingShaderParams.vertexPositionAttribLocation = InvalidLocation;
    renderingShaderParams.vertexNormalAttribLocation = InvalidLocation;
    renderingShaderParams.vertexTexCoordAttribLocation = InvalidLocation;
    renderingShaderParams.ambientColorLocation = InvalidLocation;
    renderingShaderParams.diffuseColorLocation = InvalidLocation;
    renderingShaderParams.specularColorLocation = InvalidLocation;
    renderingShaderParams.emissiveColorLocation = InvalidLocation;
    renderingShaderParams.shininessLocation = InvalidLocation;
    renderingShaderParams.showSpecularHighlightLocation = InvalidLocation;
    renderingShaderParams.textureSubRoutineUniform = 0;
    renderingShaderParams.colorSubRoutineUniform = 0;
    renderingShaderParams.lightingSubRoutineUniform = 0;
    
    renderingShaderParamsAlt.textureSamplerLocation = InvalidLocation;
    renderingShaderParamsAlt.MVPMatrixLocation = InvalidLocation;
    renderingShaderParamsAlt.MMatrixLocation = InvalidLocation;
    renderingShaderParamsAlt.VMatrixLocation = InvalidLocation;
    renderingShaderParamsAlt.oldCodeTextureSelection = 0;
    renderingShaderParamsAlt.oldCodeLightingSelection = 0;
    renderingShaderParamsAlt.emissiveColorLocation = InvalidLocation;
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
//	hasBeenInitialized = true;
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
    std::cout << "\n version: " << version;
#endif
#endif
    
    glGenVertexArrays(1, &vao);
    CHECK_GL_ERROR;
    glBindVertexArray(vao);
    CHECK_GL_ERROR;

    isUsingSubRoutines = true;
    if (!renderingShader.CreateProgram("shader.vs", "shader.fs"))
    {
        renderingShader.DestroyProgram();
        isUsingSubRoutines = false;
        if (!renderingShader.CreateProgram("ShadowMappingVert.glsl", "ShadowMappingFrag.glsl")) {
            return false;
        }
        
        if (!depthShader.CreateProgram("DepthRTTVert.glsl", "DepthRTTFrag.glsl")) {
            return false;
        }
    }
	
    renderingShader.UseProgram();
    if (isUsingSubRoutines) {
        renderingShader.CacheSubroutineUniforms();
    }

    if (isUsingSubRoutines) {
        renderingShaderParams.textureSamplerLocation = GetUniformLocation("uSampler");
        renderingShaderParams.projectionMatrixLocation = GetUniformLocation("uProjectionMatrix");
        renderingShaderParams.modelMatrixLocation = GetUniformLocation("uModelMatrix");
        renderingShaderParams.viewMatrixLocation =  GetUniformLocation("uViewMatrix");
        
        renderingShaderParams.vertexPositionAttribLocation = GetAttribLocation("aVertexPosition");
        renderingShaderParams.vertexNormalAttribLocation = GetAttribLocation("aVertexNormal");
        renderingShaderParams.vertexTexCoordAttribLocation = GetAttribLocation("aTextureCoord");
        
        renderingShaderParams.showSpecularHighlightLocation = GetUniformLocation("uShowSpecularHighlights");
        renderingShaderParams.ambientColorLocation = GetUniformLocation("uAmbientColor");
        renderingShaderParams.diffuseColorLocation = GetUniformLocation("uPointLightingDiffuseColor");
        renderingShaderParams.specularColorLocation = GetUniformLocation("uPointLightingSpecularColor");
        renderingShaderParams.emissiveColorLocation = GetUniformLocation("uPointLightingEmissiveColor");
        renderingShaderParams.shininessLocation = GetUniformLocation("uMaterialShininess");
        
        renderingShaderParams.textureSubRoutineUniform = GetSubroutineUniformIndex("textureRender", GL_FRAGMENT_SHADER);
        renderingShaderParams.colorSubRoutineUniform = GetSubroutineUniformIndex("colorRender", GL_FRAGMENT_SHADER);
        renderingShaderParams.lightingSubRoutineUniform = GetSubroutineUniformIndex("lightingRender", GL_FRAGMENT_SHADER);

        GLint o = 0;
        glGetProgramStageiv(renderingShader.GetShaderProgram(), GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &o);
        CHECK_GL_ERROR;

        renderingShaderParams.useTextureSubFunctionLocation = GetSubroutineIndex("RenderWithTexture", GL_FRAGMENT_SHADER);
        renderingShaderParams.noTextureSubFunctionLocation = GetSubroutineIndex("RenderNOTexture", GL_FRAGMENT_SHADER);
        renderingShaderParams.noLightingSubFunctionLocation = GetSubroutineIndex("RenderNoLighting", GL_FRAGMENT_SHADER);

#ifdef _DEBUG
        renderingShaderParams.useLightingSubFunctionLocation = GetSubroutineIndex("CalculateSceneLighting_DebugBuild", GL_FRAGMENT_SHADER);
        renderingShader.SetSubroutineUniformIndex(renderingShaderParams.colorSubRoutineUniform, GetSubroutineIndex("CalculateFragmentColor_DebugBuild", GL_FRAGMENT_SHADER), GL_FRAGMENT_SHADER, false, false);
#else
        renderingShaderParams.useLightingSubFunctionLocation = GetSubroutineIndex("CalculateSceneLighting_ReleaseBuild", GL_FRAGMENT_SHADER);
        renderingShader.SetSubroutineUniformIndex(colorSubRoutineUniform, GetSubroutineIndex("CalculateFragmentColor_ReleaseBuild", GL_FRAGMENT_SHADER), GL_FRAGMENT_SHADER, false, false);
#endif
        renderingShader.SetSubroutineUniformIndex(renderingShaderParams.textureSubRoutineUniform, renderingShaderParams.noTextureSubFunctionLocation, GL_FRAGMENT_SHADER, false, false);
        renderingShader.SetSubroutineUniformIndex(renderingShaderParams.lightingSubRoutineUniform, renderingShaderParams.useLightingSubFunctionLocation, GL_FRAGMENT_SHADER, true, true); // <== last one has true for forceFlush
    }
    else
    {
        renderingShaderParamsAlt.textureSamplerLocation = GetUniformLocation("myTextureSampler");
        renderingShaderParamsAlt.MVPMatrixLocation = GetUniformLocation("MVP");
        renderingShaderParamsAlt.MMatrixLocation = GetUniformLocation("M");
        renderingShaderParamsAlt.VMatrixLocation = GetUniformLocation("V");
        
        renderingShaderParamsAlt.oldCodeTextureSelection = GetUniformLocation("uTextureRender");
        //SetUniformBool(renderingShaderParamsAlt.oldCodeTextureSelection, false);
        
        renderingShaderParamsAlt.oldCodeLightingSelection = GetUniformLocation("uLightingRender");
        //SetUniformBool(renderingShaderParamsAlt.oldCodeLightingSelection, true);
        
        renderingShaderParamsAlt.emissiveColorLocation = GetUniformLocation("uPointLightingEmissiveColor");
    }
	return true;
}

void OpenGLImplementationBase::DestroyGL()
{
	renderingShader.DestroyProgram();
    depthShader.DestroyProgram();
    
    if (0 != vao)
    {
        glDeleteVertexArrays(1, &vao);
        CHECK_GL_ERROR;
        vao = 0;
    }
}

//bool OpenGLImplementationBase::HasBeenInitialized()
//{
//	return hasBeenInitialized;
//}

//void OpenGLImplementationBase::UseProgram()
//{
//	shader.UseProgram();
//}

//void OpenGLImplementationBase::StopUsingProgram()
//{
//	glUseProgram(0);
//	CHECK_GL_ERROR;
//}

//int OpenGLImplementationBase::GetShaderProgram()
//{
//	return shader.GetShaderProgram();
//}

int OpenGLImplementationBase::GetUniformLocation(const char* pUniformVariableName)
{
	return renderingShader.GetUniformLocation(pUniformVariableName);
}

unsigned int OpenGLImplementationBase::GetSubroutineUniformIndex(const char* pSubroutineUniformVariableName, unsigned int shaderType)
{
	return renderingShader.GetSubroutineUniformIndex(pSubroutineUniformVariableName, shaderType);
}

void OpenGLImplementationBase::SetSubroutineUniformIndex(int subRoutineUniformIndex, unsigned int subRoutineIndex, unsigned int shaderType)
{
	renderingShader.SetSubroutineUniformIndex(subRoutineUniformIndex, subRoutineIndex, shaderType, true, false);
}

int OpenGLImplementationBase::GetSubroutineIndex(const char* pSubRoutineName, unsigned int shaderType)
{
	return renderingShader.GetSubroutineIndex(pSubRoutineName, shaderType);
}

int OpenGLImplementationBase::GetAttribLocation(const char* pUniformVariableName)
{
	return renderingShader.GetAttribLocation(pUniformVariableName);
}

void OpenGLImplementationBase::SetUniformFloat(int location, float newValue)
{
    OpenGLShader::SetUniformFloat(location, newValue);
}

void OpenGLImplementationBase::SetUniformBool(int location, bool newValue)
{
	OpenGLShader::SetUniformBool(location, newValue);
}

void OpenGLImplementationBase::SetUniformInt(int location, int newValue)
{
	OpenGLShader::SetUniformInt(location, newValue);
}

void OpenGLImplementationBase::SetUniformMatrix(int location, const CMatrix& mat)
{
    if (IsUsingSubRoutines()) {
        OpenGLShader::SetUniformMatrix(location, mat);
    } else {
        if (renderingShaderParamsAlt.MMatrixLocation == location) {
            SetModelMatrix(mat);
        } else if (renderingShaderParamsAlt.VMatrixLocation == location) {
            SetViewMatrix(mat);
        } else {
            OpenGLShader::SetUniformMatrix(location, mat);
        }
    }
}

void OpenGLImplementationBase::SetUniformVector3(int location, const CVector& vect)
{
	OpenGLShader::SetUniformVector3(location, vect);
}

void OpenGLImplementationBase::SetUniformVector4(int location, const CVector4& vect)
{
	OpenGLShader::SetUniformVector4(location, vect);
}

void OpenGLImplementationBase::UseTextureDefault(int textureIndex)
{
    if (isUsingSubRoutines) {
        UseTexture(0, textureIndex, renderingShaderParams.textureSamplerLocation);
        SetSubroutineUniformIndex(renderingShaderParams.textureSubRoutineUniform, renderingShaderParams.useTextureSubFunctionLocation, GL_FRAGMENT_SHADER);
    }
    else
    {
        UseTexture(0, textureIndex, renderingShaderParamsAlt.textureSamplerLocation);
        SetUniformBool(renderingShaderParamsAlt.oldCodeTextureSelection, true);
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
        SetSubroutineUniformIndex(renderingShaderParams.textureSubRoutineUniform, renderingShaderParams.noTextureSubFunctionLocation, GL_FRAGMENT_SHADER);
    }
    else
    {
        SetUniformBool(renderingShaderParamsAlt.oldCodeTextureSelection, false);
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
    glDisableVertexAttribArray(IsUsingSubRoutines() ? renderingShaderParams.vertexPositionAttribLocation : renderingShaderParamsAlt.vertexPositionAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::SetVertexPositionAttribPointer(int stride, int offset)
{
	const int NumComponents = 3;
	VertexAttribPointer(IsUsingSubRoutines() ? renderingShaderParams.vertexPositionAttribLocation : renderingShaderParamsAlt.vertexPositionAttribLocation, NumComponents, stride, offset);
}

void OpenGLImplementationBase::DisableVertexNormalAttribPointer()
{
	glDisableVertexAttribArray(IsUsingSubRoutines() ? renderingShaderParams.vertexNormalAttribLocation : renderingShaderParamsAlt.vertexNormalAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::SetVertexNormalAttribPointer(int stride, int offset)
{
	const int NumComponents = 3;
    VertexAttribPointer(IsUsingSubRoutines() ? renderingShaderParams.vertexNormalAttribLocation : renderingShaderParamsAlt.vertexNormalAttribLocation, NumComponents, stride, offset);
}

void OpenGLImplementationBase::DisableVertexTextureAttribPointer()
{
	glDisableVertexAttribArray(IsUsingSubRoutines() ? renderingShaderParams.vertexTexCoordAttribLocation : renderingShaderParamsAlt.vertexTexCoordAttribLocation);
	CHECK_GL_ERROR;
}

void OpenGLImplementationBase::SetVertexTextureAttribPointer(int stride)
{
	const int NumComponents = 2;
	VertexAttribPointer(IsUsingSubRoutines() ? renderingShaderParams.vertexTexCoordAttribLocation : renderingShaderParamsAlt.vertexTexCoordAttribLocation, NumComponents, stride);
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
    if (isUsingSubRoutines) {
        SetUniformMatrix(renderingShaderParams.projectionMatrixLocation, mat);
    } else {
        renderingShaderParamsAlt.projectionMatrix = mat;
        CMatrix MVP = renderingShaderParamsAlt.projectionMatrix * renderingShaderParamsAlt.lookAtViewMatrix * renderingShaderParamsAlt.modelMatrix;
        OpenGLShader::SetUniformMatrix(renderingShaderParamsAlt.MVPMatrixLocation, MVP);
    }
}

void OpenGLImplementationBase::SetModelMatrix(const CMatrix& mat)
{
    if (isUsingSubRoutines) {
        SetUniformMatrix(renderingShaderParams.modelMatrixLocation, mat);
    } else {
        renderingShaderParamsAlt.modelMatrix = mat;
        OpenGLShader::SetUniformMatrix(renderingShaderParamsAlt.MMatrixLocation, mat);
        SetProjectionMatrix(renderingShaderParamsAlt.projectionMatrix);
    }
}

void OpenGLImplementationBase::SetViewMatrix(const CMatrix& mat)
{
    if (isUsingSubRoutines) {
        SetUniformMatrix(renderingShaderParams.viewMatrixLocation, mat);
    } else {
        renderingShaderParamsAlt.lookAtViewMatrix = mat;
        OpenGLShader::SetUniformMatrix(renderingShaderParamsAlt.VMatrixLocation, mat);
        SetProjectionMatrix(renderingShaderParamsAlt.projectionMatrix);
    }
}

void OpenGLImplementationBase::SetAmbientColor(const CVector& color)
{
    if (IsUsingSubRoutines()) {
        SetUniformVector3(renderingShaderParams.ambientColorLocation, color);
    }
}

void OpenGLImplementationBase::SetDiffuseColor(const CVector& color)
{
    if (IsUsingSubRoutines()) {
        SetUniformVector3(renderingShaderParams.diffuseColorLocation, color);
    }
}

void OpenGLImplementationBase::SetSpecularColor(const CVector& color)
{
    if (IsUsingSubRoutines()) {
        SetUniformVector3(renderingShaderParams.specularColorLocation, color);
    }
}

void OpenGLImplementationBase::SetEmissiveColor(const CVector4& color)
{
    if (IsUsingSubRoutines()) {
        SetUniformVector4(renderingShaderParams.emissiveColorLocation, color);
    } else {
        SetUniformVector4(renderingShaderParamsAlt.emissiveColorLocation, color);
    }
}

void OpenGLImplementationBase::SetShininess(float val)
{
    if (IsUsingSubRoutines()) {
        SetUniformFloat(renderingShaderParams.shininessLocation, val);
    }
}

void OpenGLImplementationBase::ShowSpecularHighlights(bool show)
{
    if (IsUsingSubRoutines()) {
        SetUniformBool(renderingShaderParams.showSpecularHighlightLocation, show);
    }
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
                SetSubroutineUniformIndex(renderingShaderParams.lightingSubRoutineUniform, renderingShaderParams.useLightingSubFunctionLocation, GL_FRAGMENT_SHADER);
            }
            else
            {
                SetUniformBool(renderingShaderParamsAlt.oldCodeLightingSelection, true);
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
                SetSubroutineUniformIndex(renderingShaderParams.lightingSubRoutineUniform, renderingShaderParams.noLightingSubFunctionLocation, GL_FRAGMENT_SHADER);
            }
            else
            {
                SetUniformBool(renderingShaderParamsAlt.oldCodeLightingSelection, false);
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