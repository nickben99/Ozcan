#ifdef USE_SHADERS
// Text.h

//system includes------------
//---------------------------

#ifndef _Text_h_
#define _Text_h_

//header files---------
#include "Rendering\MeshBuffer.h"
#include "Math\CVector.h"
#include "Math\CMatrix.h"
//---------------------

//defines--------------
//---------------------

//forward declerations----
//------------------------

class Text
{
public:
	enum HorizontalAlignment
	{
		left = 0, 
		right, 
		center
	};

	enum VirticalAlignment
	{
		top = 0, 
		bottom, 
		middle
	};

	static void CreateMesh();
	static void DeleteMesh();

	Text();
	Text(const char* text, float xPos, float yPos, HorizontalAlignment hAlign, VirticalAlignment vAlign, const CVector4& color, float size);
	~Text();

	void SetText(const char* text, float xPos, float yPos, HorizontalAlignment hAlign, VirticalAlignment vAlign, const CVector4& color, float size);
	void SetText(const char* text, float xPos, float yPos, HorizontalAlignment hAlign, VirticalAlignment vAlign, const CVector4& color);
	void SetText(float xPos, float yPos, HorizontalAlignment hAlign, VirticalAlignment vAlign, const CVector4& color);
	void SetSize(float size);
	void SetColor(const CVector4& color);
	void SetText(const char* text, ...);
	void SetText(float xPos, float yPos);
	void SetText(float xPos, float yPos, const char* text, ...);

	void GetTextBox(CVector& topLeft, CVector& bottomRight);
	float GetTextHeight();
	float GetTextWidth();

	void Draw();

private:
	void Init();

private:
	static const int BufferLength = 256;
	
	static MeshBuffer textMesh;
	static bool hasTextMeshBeenInitialized;
	static int textSheet;

	char currentText[BufferLength];
	CMatrix alignment;

	float xPos;
	float yPos;
	HorizontalAlignment horizontalAlign;
	VirticalAlignment virticalAlign; 
	CVector4 col;
	float size;
};

#endif // ifndef _Text_h_ 
#endif // #ifdef USE_SHADERS