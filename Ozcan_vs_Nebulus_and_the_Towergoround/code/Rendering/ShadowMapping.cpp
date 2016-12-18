//ShadowMapping.cpp

#ifdef USE_SHADERS

// system includes --------
#ifdef _WINDOWS
#include <windows.h>	// header file for windows
#include <glew-1.12.0\include\GL/glew.h>
#include <gl\glu.h>		// header file for the GLu32 library
#endif
#ifdef OSX
#include </usr/local/Cellar/glew/1.13.0/include/GL/glew.h>
#endif
//-------------------------

// header files -----------
#include "ShadowMapping.h"
#include "FileReading/TEXTURE.H"
#include "Game/Globals.h"
#include "Game/CMenu.h"
#include "Math/CBox.h"
#ifdef _DEBUG
	#include "Debug/DebugMenu.h"
#endif
//-------------------------

//--- external variables --------
//-------------------------------

//---static variable declerations ---
unsigned int ShadowMapping::frameBuffer = 0;
unsigned int ShadowMapping::depthTexture = 0;

int ShadowMapping::shadowMappingVertexSubroutineUniform = -1;
int ShadowMapping::shadowMappingVertexRenderSubRoutineIndex = -1;
int ShadowMapping::mainRenderVertexSubRoutineIndex = -1;
int ShadowMapping::oldCodeVertexRenderSelector = -1;

int ShadowMapping::shadowMappingSubroutineUniform = -1;
int ShadowMapping::shadowMappingRenderSubRoutineIndex = -1;
int ShadowMapping::mainRenderSubRoutineIndex = -1;
int ShadowMapping::oldCodeFragmentRenderSelector = -1;

int ShadowMapping::lightViewProjectionMatrixUniform = -1;
int ShadowMapping::depthTextureSamplerUniform = -1;
//-----------------------------------

ShadowMapping::ShadowMapping()
{
}

ShadowMapping::~ShadowMapping()
{
}

const float defaultHorizontalProjection = 13.0f; 
const float defaultVirticalProjection = 37.75f;
const float defaultForwardProjection = 100.0f;

float horizontalProjection = defaultHorizontalProjection; 
float virticalProjection = defaultVirticalProjection;
float forwardProjection = defaultForwardProjection;

bool wholeSceneShadowMap = false;
int textureSize = 1024;

#if (_DEBUG && USE_SHADERS)
bool resetProjectionValues = false;
bool shadowMappingVariablesAddedToDebugMenu = false;
#endif

unsigned int ShadowMapping::Create()
{
	Destroy(); // delete anything hanging around
	
    if (Globals::Instance().gl.IsUsingSubRoutines())
    {
        shadowMappingVertexSubroutineUniform =  Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_VERTEX_SHADER);
        shadowMappingVertexRenderSubRoutineIndex = Globals::Instance().gl.GetSubroutineIndex("MainShadowMapCreation", GL_VERTEX_SHADER);
        mainRenderVertexSubRoutineIndex = Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_VERTEX_SHADER);

        shadowMappingSubroutineUniform = Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_FRAGMENT_SHADER);
        shadowMappingRenderSubRoutineIndex = Globals::Instance().gl.GetSubroutineIndex("MainShadowMapCreation", GL_FRAGMENT_SHADER);
        mainRenderSubRoutineIndex = Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_FRAGMENT_SHADER);

        lightViewProjectionMatrixUniform = Globals::Instance().gl.GetUniformLocation("uViewProjectionLightMatrix");
        depthTextureSamplerUniform = Globals::Instance().gl.GetUniformLocation("uShadowMap");
    }
    else
    {
        oldCodeVertexRenderSelector = Globals::Instance().gl.GetUniformLocation("uMainRenderVertexShader");
        oldCodeFragmentRenderSelector = Globals::Instance().gl.GetUniformLocation("uMainRenderFragmentShader");
    }

#if (_DEBUG && USE_SHADERS)
	if (!shadowMappingVariablesAddedToDebugMenu)
	{
		shadowMappingVariablesAddedToDebugMenu = true;
		DebugMenu::DebugMenuItemContainer* container = new DebugMenu::DebugMenuItemContainer("Shadow Mapping");
		container->AddVariable(new DebugMenu::FloatDebugMenuItem("horizontal proj", &horizontalProjection, 1.0f, -1.0f));
		container->AddVariable(new DebugMenu::FloatDebugMenuItem("virtical proj", &virticalProjection, 1.0f, -1.0f));
		container->AddVariable(new DebugMenu::FloatDebugMenuItem("forward proj", &forwardProjection, 1.0f, -1.0f));

		container->AddVariable(new DebugMenu::BoolDebugMenuItem("wholeSceneShadowMap", &wholeSceneShadowMap));
		container->AddVariable(new DebugMenu::BoolDebugMenuItem("resetProjectionValues", &resetProjectionValues));

		Globals::Instance().debugMenu->AddVariable(container);
	}
#endif

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // start using our new frame buffer

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	depthTexture = getTextureNumber("shadow mapping texture");
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, textureSize, textureSize, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	
	UseDefaultFrameBuffer();
	return true;
}

void ShadowMapping::UseDefaultFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER,0); // go back to using the default frame buffer
}

void ShadowMapping::Destroy()
{
	if (0 != depthTexture)
	{
		deleteTexture(depthTexture);
		depthTexture = 0;
	}

	if (0 != frameBuffer)
	{
		glDeleteFramebuffers(1, &frameBuffer);
		frameBuffer = 0;
	}	
}

void ShadowMapping::PreDepthTextureRender()
{
#ifdef _DEBUG
	if (resetProjectionValues)
	{
		resetProjectionValues = false;
		horizontalProjection = defaultHorizontalProjection; 
		virticalProjection = defaultVirticalProjection;
		forwardProjection = defaultForwardProjection;
	}
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0,0,textureSize,textureSize); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    if (Globals::Instance().gl.IsUsingSubRoutines())
    {
        Globals::Instance().gl.SetSubroutineUniformIndex(shadowMappingSubroutineUniform, shadowMappingRenderSubRoutineIndex, GL_FRAGMENT_SHADER);
        Globals::Instance().gl.SetSubroutineUniformIndex(shadowMappingVertexSubroutineUniform, shadowMappingVertexRenderSubRoutineIndex, GL_VERTEX_SHADER);
    }
    else
    {
        Globals::Instance().gl.SetUniformBool(oldCodeVertexRenderSelector, true);
        Globals::Instance().gl.SetUniformBool(oldCodeFragmentRenderSelector, true);
    }

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Globals::Instance().gl.SetProjectionMatrix(CalculateLightOrthographicProjection());

	Globals::Instance().viewMatrixStack.PushMatrix();
	Globals::Instance().viewMatrixStack.LoadIdentity();
	Globals::Instance().viewMatrixStack.MultMatrix(CalculateLightLookAtMatrix());

	Globals::Instance().modelMatrixStack.PushMatrix();
	Globals::Instance().modelMatrixStack.LoadIdentity();
}

void ShadowMapping::PostDepthTextureRender()
{
	Globals::Instance().modelMatrixStack.PopMatrix();
	Globals::Instance().viewMatrixStack.PopMatrix();
    
    if (Globals::Instance().gl.IsUsingSubRoutines())
    {
        CMenu::SetPerspectiveProjectionMatrix();
        Globals::Instance().gl.SetSubroutineUniformIndex(shadowMappingSubroutineUniform, mainRenderSubRoutineIndex, GL_FRAGMENT_SHADER);
        Globals::Instance().gl.SetSubroutineUniformIndex(shadowMappingVertexSubroutineUniform, mainRenderVertexSubRoutineIndex, GL_VERTEX_SHADER);
    }
    else
    {
        Globals::Instance().gl.SetUniformBool(oldCodeVertexRenderSelector, false);
        Globals::Instance().gl.SetUniformBool(oldCodeFragmentRenderSelector, false);
    }

	glViewport(0, 0, (int)Globals::Instance().windowWidth, (int)Globals::Instance().windowHeight); 
	UseDefaultFrameBuffer();
}

void ShadowMapping::PreSceneRender()
{
	CMatrix biasMatrix(
		CVector4(0.5, 0.0, 0.0, 0.0), 
		CVector4(0.0, 0.5, 0.0, 0.0),
		CVector4(0.0, 0.0, 0.5, 0.0),
		CVector4(0.5, 0.5, 0.5, 1.0)
	);

	CMatrix lightMVP = biasMatrix * CalculateLightOrthographicProjection() * CalculateLightLookAtMatrix();

	Globals::Instance().gl.SetUniformMatrix(lightViewProjectionMatrixUniform, lightMVP);
	Globals::Instance().gl.UseTexture(1, depthTexture, depthTextureSamplerUniform); 
}

void ShadowMapping::PostSceneRender()
{
}

CMatrix ShadowMapping::CalculateLightOrthographicProjection()
{
	if (wholeSceneShadowMap)
	{
		CVector lightPos = Globals::Instance().currentLevel.levelOctree.centre; // the centre of the world	
		CBox sceneBox(lightPos, Globals::Instance().currentLevel.levelOctree.geometryBoxWidth);
	
		CMatrix lightMat(Globals::Instance().currentLevel.theScene.GetLightDirection());

		// the entire scene must fit on the shadow map texture
		horizontalProjection = sceneBox.CalculateMaxProjectionOnToLine(lightPos, lightMat.right());
		virticalProjection = sceneBox.CalculateMaxProjectionOnToLine(lightPos, lightMat.up());
		forwardProjection = sceneBox.CalculateMaxProjectionOnToLine(lightPos, lightMat.forward());
	}
	else
	{
		CVector lightPos = Globals::Instance().currentLevel.levelOctree.centre; // the centre of the world	
		lightPos.y = CCamera::instance()->GetActiveCameraPosition().y;
		CBox sceneBox(lightPos, Globals::Instance().currentLevel.levelOctree.geometryBoxWidth);// + CVector::one);
	
		CMatrix lightMat(Globals::Instance().currentLevel.theScene.GetLightDirection());
		horizontalProjection = sceneBox.CalculateMaxProjectionOnToLine(lightPos, lightMat.right());
	}

	return CMatrix::CreateOrthographicProjection(-horizontalProjection, horizontalProjection, 
												-virticalProjection, virticalProjection, 
												-forwardProjection, forwardProjection);
}

CMatrix ShadowMapping::CalculateLightLookAtMatrix()
{
	CVector lightPos = Globals::Instance().currentLevel.levelOctree.centre; // the centre of the world
	
	if (!wholeSceneShadowMap)
	{
		lightPos.y = CCamera::instance()->GetActiveCameraPosition().y;
	}

	CVector lightDir = Globals::Instance().currentLevel.theScene.GetLightDirection();
	return CMatrix::LookAt(lightPos, lightPos + lightDir, CVector::unitY);
}

#endif // #ifdef USE_SHADERS