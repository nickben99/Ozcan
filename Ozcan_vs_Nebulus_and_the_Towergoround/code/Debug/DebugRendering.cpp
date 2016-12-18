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
	boxMesh.Delete();
}

void DebugRendering::InitShapes()
{
	debugRenderingUniform = Globals::Instance().gl.GetUniformLocation("uIsDebugRendering");
	InitBox();
}

void DebugRendering::InitBox()
{
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
}

void DebugRendering::DrawBox(const CVector& pos, float radius, const CVector4& color)
{
	CMatrix position;
	position.creatScalingMatrix(radius, radius, radius);
	position.SetMatrixTranslation(CVector4(pos, 1.0f));

	DrawBox(position, color);
}

void DebugRendering::DrawBox(const CMatrix& trans, const CVector4& color)
{
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
}

#endif // #if _DEBUG