// DebugRendering.h

#if _DEBUG

//system includes------------
//---------------------------

#ifndef _DebugRendering_h_
#define _DebugRendering_h_

//header files---------
#ifdef _WINDOWS
#include "Rendering\MeshBuffer.h"
#endif
//---------------------

//defines--------------
//---------------------

//forward declerations----
class CVector;
class CVector4;
class CMatrix;
//------------------------

class DebugRendering
{
public:
	DebugRendering();
	~DebugRendering();

	void DrawBox(const CVector& pos, float radius, const CVector4& color);
	void DrawBox(const CMatrix& trans, const CVector4& color);

	void InitShapes();
	void DestroyShapes();

private:
	void InitBox();

private:
#ifdef _WINDOWS
	MeshBuffer boxMesh;
#endif
	int debugRenderingUniform;
};

#endif // ifndef _DebugRendering_h_ 
#endif // #if _DEBUG