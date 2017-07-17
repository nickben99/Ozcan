//MeshBuffer.h

#ifdef USE_SHADERS

#ifndef _MeshBuffer_h_
#define _MeshBuffer_h_

//system includes------------
#include <assert.h>
//---------------------------

//header files---------
//---------------------

//defines--------------

//---------------------

//forward declerations----
class CVector4;
struct material;
//------------------------

class MeshBuffer
{
public:
	enum eFormat
	{
		SeperateArrays, 
		SingleArray
	};

	MeshBuffer(eFormat format = eFormat::SeperateArrays);
	~MeshBuffer();

	void Delete();
	static const int DrawAll = -1;
	void Draw(int numVertsToDraw = DrawAll);

	void CreateVertexArray(float* pArray, int numElements, bool isDynamic = false);
	void UpdateVertexArray(float* pArray);

	void CreateNormalArray(float* pArray, int numElements, bool isDynamic = false);
	void UpdateNormalArray(float* pArray);
	
	void SetColor(const CVector4& color);
	void SetMaterial(const material& meshMaterial);
	void SetTexture(int textureInput);
	
	void CreateTexCoordArray(float* pArray, int numElements, bool isDynamic = false);
	void UpdateTexCoordArray(float* pArray);

	void CreateIndexArray(unsigned short* pArray, int numElements);			

private:
	void DrawSingleArray(int numVertsToDraw);
	void DrawSeperateArrays(int numVertsToDraw);	
	void EnableMaterialColors();
	void EnableTexture();
	void DrawBuffer(int numVertsToDraw);
	void CalculateNumTriangleVertsToDraw();

	MeshBuffer& operator=(const MeshBuffer&)
	{
		assert(false);
		return *this;
	}

public:
	int vertexArrayBuffer,
		normalArrayBuffer,
		texCoordArrayBuffer,
		indexArrayBuffer;

	int vertexArrayBufferLength,
		normalArrayBufferLength,
		texCoordArrayBufferLength,
		indexArrayBufferLength;

	int numTriangleVertsToDraw;

	const eFormat format;

	float	ambient[4],
			diffuse[4],
			specular[4],
			emissive[4],
			shinnines;
	int texture;
};

#endif // ifndef _MeshBuffer_h_ 

#endif // ifdef USE_SHADERS