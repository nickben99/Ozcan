// DebugRendering.h

#if _DEBUG

//system includes------------
//---------------------------

#ifndef _DebugRendering_h_
#define _DebugRendering_h_

//header files---------
#include "Rendering/MeshBuffer.h"
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

	void InitShapesMac();
	void DestroyShapesMac();

private:
	void InitBox();

private:
	MeshBuffer boxMesh;
	int debugRenderingUniform;
};

#endif // ifndef _DebugRendering_h_ 
#endif // #if _DEBUG