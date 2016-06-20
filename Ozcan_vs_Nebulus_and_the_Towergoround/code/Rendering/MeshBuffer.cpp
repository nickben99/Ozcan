//MeshBuffer.cpp

#ifdef USE_SHADERS

// system includes --------
#include <glew-1.12.0\include\GL/glew.h>
//-------------------------

// header files -----------
#include "MeshBuffer.h"
#include "Game\Globals.h"
#include "Rendering\CModel.h"
//-------------------------

//--- external variables --------
//-------------------------------

//---static variable declerations ---
//-----------------------------------

const int NumColorElements = 4;

// constructor
MeshBuffer::MeshBuffer(eFormat inputFormat)
	: vertexArrayBuffer(-1)
	, normalArrayBuffer(-1)
	, texCoordArrayBuffer(-1)
	, indexArrayBuffer(-1)

	, vertexArrayBufferLength(0)
	, normalArrayBufferLength(0)
	, texCoordArrayBufferLength(0)
	, indexArrayBufferLength(0)

	, numTriangleVertsToDraw(0)

	, format(inputFormat)

	, shinnines(0.0f)
	, texture(-1)
{	
	for (int element = 0; element < NumColorElements; ++element)
	{
		ambient[element] = 1.0f;
		diffuse[element] = 1.0f;
		specular[element] = 1.0f;
		emissive[element] = 1.0f;
	}
}

void MeshBuffer::Delete()
{
	if (-1 != vertexArrayBuffer)
	{
		Globals::Instance().gl.DeleteVertexBufferObject(vertexArrayBuffer);
		vertexArrayBuffer = -1;
	}

	if (-1 != normalArrayBuffer)
	{
		Globals::Instance().gl.DeleteVertexBufferObject(normalArrayBuffer);
		normalArrayBuffer = -1;
	}

	if (-1 != texCoordArrayBuffer)
	{
		Globals::Instance().gl.DeleteVertexBufferObject(texCoordArrayBuffer);
		texCoordArrayBuffer = -1;
	}

	if (-1 != indexArrayBuffer)
	{
		Globals::Instance().gl.DeleteVertexBufferObject(indexArrayBuffer);
		indexArrayBuffer = -1;
	}
}

// destructor
MeshBuffer::~MeshBuffer()
{
	Delete();
}

void MeshBuffer::Draw(int numVertsToDraw)
{
	switch (format)
	{
	case eFormat::SeperateArrays:
		DrawSeperateArrays(numVertsToDraw);
		break;
	case eFormat::SingleArray:
		DrawSingleArray(numVertsToDraw);
		break;
	default: break;
	}
}

void MeshBuffer::EnableMaterialColors()
{	
	Globals::Instance().gl.GLMaterial(GL_AMBIENT, ambient);
	Globals::Instance().gl.GLMaterial(GL_DIFFUSE, diffuse);
	Globals::Instance().gl.GLMaterial(GL_SPECULAR, specular);
	Globals::Instance().gl.GLMaterial(GL_EMISSION, emissive);
	
	Globals::Instance().gl.GLMaterial(GL_SHININESS, shinnines);
}

void MeshBuffer::EnableTexture()
{
	if (-1 != texture)
	{
		// set texture required
		Globals::Instance().gl.UseTextureDefault(texture);
	}
	else
	{
		Globals::Instance().gl.DontUseDefaultTexture();
	}
}

void MeshBuffer::DrawBuffer(int numVertsToDraw)
{	
	if (-1 != indexArrayBuffer)
	{
		// indecis
		Globals::Instance().gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrayBuffer);
	
		// draw
		Globals::Instance().gl.DrawElements((DrawAll == numVertsToDraw) ? indexArrayBufferLength : numVertsToDraw);	
	}
	else
	{
		Globals::Instance().gl.DrawArrays((DrawAll == numVertsToDraw) ? numTriangleVertsToDraw : numVertsToDraw);
	}
}

void MeshBuffer::DrawSingleArray(int numVertsToDraw)
{
	EnableMaterialColors();
	EnableTexture();

	int vertSize = sizeof(CVector);
	int normalSize = sizeof(CVector);
	int textureCoordSize = (2*sizeof(float));

	int stride = vertSize + normalSize + textureCoordSize; 

    int normalOffset = textureCoordSize;
	int vertOffset = textureCoordSize + normalSize;

    Globals::Instance().gl.BindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
    Globals::Instance().gl.SetVertexTextureAttribPointer(stride);
    Globals::Instance().gl.SetVertexNormalAttribPointer(stride, normalOffset);
    Globals::Instance().gl.SetVertexPositionAttribPointer(stride, vertOffset);

	DrawBuffer(numVertsToDraw);

	Globals::Instance().gl.DisableVertexTextureAttribPointer();
    Globals::Instance().gl.DisableVertexNormalAttribPointer();
    Globals::Instance().gl.DisableVertexPositionAttribPointer();
}

void MeshBuffer::DrawSeperateArrays(int numVertsToDraw)
{
	Globals::Instance().gl.BindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
	Globals::Instance().gl.SetVertexPositionAttribPointer();
	
	EnableMaterialColors(); // colors
	
	if (-1 != normalArrayBuffer)
	{
		// normals
		Globals::Instance().gl.BindBuffer(GL_ARRAY_BUFFER, normalArrayBuffer);
		Globals::Instance().gl.SetVertexNormalAttribPointer();
	}

	if (-1 != texCoordArrayBuffer)
	{
		// tex coords	
		Globals::Instance().gl.BindBuffer(GL_ARRAY_BUFFER, texCoordArrayBuffer);
		Globals::Instance().gl.SetVertexTextureAttribPointer();
	}

	EnableTexture();	
	DrawBuffer(numVertsToDraw);

	if (-1 != texCoordArrayBuffer)
	{
		Globals::Instance().gl.DisableVertexTextureAttribPointer();
	}

	if (-1 != normalArrayBuffer)
	{
		Globals::Instance().gl.DisableVertexNormalAttribPointer();
	}
    Globals::Instance().gl.DisableVertexPositionAttribPointer();
}

void MeshBuffer::CalculateNumTriangleVertsToDraw()
{
	switch (format)
	{
	case eFormat::SeperateArrays:
		{
			const int NumElementsPerVert = 3; // just the vert
			numTriangleVertsToDraw = vertexArrayBufferLength / NumElementsPerVert;
			break;
		}
	case eFormat::SingleArray:
		{
			const int NumElementsPerVert = 8; // vert, normal, text coords
			numTriangleVertsToDraw = vertexArrayBufferLength / NumElementsPerVert;
			break;
		}
	default: break;
	}
}

void MeshBuffer::CreateVertexArray(float* pArray, int numElements, bool isDynamic)
{
	vertexArrayBufferLength = numElements;
	CalculateNumTriangleVertsToDraw();
	vertexArrayBuffer = Globals::Instance().gl.CreateVertexBufferObject<float>(pArray, numElements, GL_ARRAY_BUFFER, isDynamic);
}

void MeshBuffer::UpdateVertexArray(float* pArray)
{
	Globals::Instance().gl.UpdateVertexBufferObject(vertexArrayBuffer, pArray, vertexArrayBufferLength, GL_ARRAY_BUFFER);
}

void MeshBuffer::CreateNormalArray(float* pArray, int numElements, bool isDynamic)
{
	normalArrayBufferLength = numElements;
	normalArrayBuffer = Globals::Instance().gl.CreateVertexBufferObject<float>(pArray, numElements, GL_ARRAY_BUFFER, isDynamic);
}

void MeshBuffer::UpdateNormalArray(float* pArray)
{
	Globals::Instance().gl.UpdateVertexBufferObject(normalArrayBuffer, pArray, normalArrayBufferLength, GL_ARRAY_BUFFER);
}

void MeshBuffer::SetMaterial(const material& trianglesMaterial)
{
	for (int element = 0; element < NumColorElements; ++element)
	{
		ambient[element] = trianglesMaterial.ambient[element];
		diffuse[element] = trianglesMaterial.diffuse[element];
		specular[element] = trianglesMaterial.specular[element];
		emissive[element] = trianglesMaterial.emissive[element];
	}

	shinnines = trianglesMaterial.shinnines;
	if(trianglesMaterial.clrTexBndNo > 0) // if this material has a texture
	{	// bind texture
		SetTexture(trianglesMaterial.clrTexBndNo);
	}
}

void MeshBuffer::SetTexture(int textureInput)
{
	texture = textureInput;
}

void MeshBuffer::SetColor(const CVector4& colorInput)
{
	emissive[0] = colorInput[0];
	emissive[1] = colorInput[1];
	emissive[2] = colorInput[2];
	emissive[3] = colorInput[3];
}

void MeshBuffer::CreateTexCoordArray(float* pArray, int numElements, bool isDynamic)
{
	texCoordArrayBufferLength = numElements;
	texCoordArrayBuffer = Globals::Instance().gl.CreateVertexBufferObject<float>(pArray, numElements, GL_ARRAY_BUFFER, isDynamic);
}

void MeshBuffer::UpdateTexCoordArray(float* pArray)
{
	Globals::Instance().gl.UpdateVertexBufferObject(texCoordArrayBuffer, pArray, texCoordArrayBufferLength, GL_ARRAY_BUFFER);
}

void MeshBuffer::CreateIndexArray(unsigned short* pArray, int numElements)
{
	indexArrayBufferLength = numElements;
	indexArrayBuffer = Globals::Instance().gl.CreateVertexBufferObject<unsigned short>(pArray, numElements, GL_ELEMENT_ARRAY_BUFFER);
}

#endif // ifdef USE_SHADERS