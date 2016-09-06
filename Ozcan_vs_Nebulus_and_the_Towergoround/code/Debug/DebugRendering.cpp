//DebugRendering.cpp

#ifdef _DEBUG

// system includes --------
//-------------------------

// header files -----------
#include "DebugRendering.h"
#include "Math/CMatrix.h"
#include "Math/CVector.h"
#include "Game/Globals.h"
//-------------------------

bool enabled = true;

DebugRendering::DebugRendering()
	: debugRenderingUniform(-1)
{
}

DebugRendering::~DebugRendering()
{
}

void DebugRendering::DestroyShapes()
{
#ifdef _WINDOWS
	boxMesh.Delete();
#endif
}

void DebugRendering::InitShapes()
{
#ifdef _WINDOWS
	debugRenderingUniform = Globals::Instance().gl.GetUniformLocation("uIsDebugRendering");
	InitBox();
#endif
}

void DebugRendering::InitBox()
{
#ifdef _WINDOWS
	boxMesh.Delete(); // in-case it has already been set-up

	 float vertexPositionArray[] = 
	 {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	boxMesh.CreateVertexArray(vertexPositionArray, 108);	

	float normals[] = 
	{
		-1.0f,0.0f,0.0f,
		-1.0f,0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		0.0f, 0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,

		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f,0.0f,
		0.0f,-1.0f,0.0f,

		0.0f, 0.0f,-1.0f,
		0.0f,0.0f,-1.0f,
		0.0f,0.0f,-1.0f,

		-1.0f,0.0f,0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f,0.0f,

		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f,0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f,0.0f, 1.0f,
		0.0f,0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f,0.0f,0.0f,
		1.0f, 0.0f,0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	boxMesh.CreateNormalArray(normals, 108);

	boxMesh.SetColor(CVector4::white);
#endif
}

void DebugRendering::DrawBox(const CVector& /* pos */, float /* radius */, const CVector4& /* color */)
{
#ifdef _WINDOWS
	CMatrix position;
	position.creatScalingMatrix(radius, radius, radius);
	position.SetMatrixTranslation(CVector4(pos, 1.0f));

	DrawBox(position, color);
#endif
}

void DebugRendering::DrawBox(const CMatrix& /* trans */, const CVector4& /* color */)
{
#ifdef _WINDOWS
	if (!enabled)
	{
		return;
	}

	boxMesh.SetColor(color);

	Globals::Instance().modelMatrixStack.PushMatrix();
	Globals::Instance().modelMatrixStack.MultMatrix(trans);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Globals::Instance().gl.SetUniformBool(debugRenderingUniform, true);
		boxMesh.Draw();
		Globals::Instance().gl.SetUniformBool(debugRenderingUniform, false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Globals::Instance().modelMatrixStack.PopMatrix();
#endif
}

#endif // #if _DEBUG