//ShadowMapping.h

#ifdef USE_SHADERS

//system includes------------
//---------------------------

#ifndef _ShadowMapping_h_
#define _ShadowMapping_h_

//header files---------
#include "Math/CMatrix.h"
//---------------------

//defines--------------

//---------------------

//forward declerations----
//------------------------

class ShadowMapping
{
public:
	ShadowMapping();
	~ShadowMapping();

	static unsigned int Create();
	static void Destroy();

	static void PreDepthTextureRender();
	static void PostDepthTextureRender();

	static void PreSceneRender();
	static void PostSceneRender();
    
    static unsigned int GetDepthTexture();

private:
	static void UseDefaultFrameBuffer();
	static CMatrix CalculateLightOrthographicProjection();
	static CMatrix CalculateLightLookAtMatrix();

	static unsigned int frameBuffer;
	static unsigned int depthTexture;

	static int shadowMappingVertexSubroutineUniform;
	static int shadowMappingVertexRenderSubRoutineIndex;
	static int mainRenderVertexSubRoutineIndex;
    static int oldCodeVertexRenderSelector;

	static int shadowMappingSubroutineUniform;
	static int shadowMappingRenderSubRoutineIndex;
	static int mainRenderSubRoutineIndex;
    static int oldCodeFragmentRenderSelector;

	static int lightViewProjectionMatrixUniform;
	static int depthTextureSamplerUniform;
};

#endif // ifndef _ShadowMapping_h_ 
#endif // #ifdef USE_SHADERS