#ifdef USE_SHADERS
//Text.cpp

// system includes --------
#include <Rendering/OpenGLInclude.h>
//-------------------------

// header files -----------
#include "Text.h"
#include "FileReading/imageFileLoaders.H"
#include "FileReading/TEXTURE.H"
#include "Game/defines.h"
#include "Game/Globals.h"
//-------------------------

//--- external variables --------
extern GLuint TextureLoad(const char *filename, bool alpha, GLenum minfilter, GLenum magfilter, GLenum wrap);
//-------------------------------

//---static variable declerations ---
const float imageWidth = 512;
const float imageHeight = 512;

class DrawPos
{
public:
	float width;
	float height;

	CVector topLeftTexCoord;
	CVector topRightTexCoord;
	CVector bottomRightTexCoord;
	CVector bottomLeftTexCoord;

	DrawPos()
	{
	}

	DrawPos(float inLeft, float inTop, float inRight, float inBottom)
		: width(inRight - inLeft)
		, height(inBottom - inTop)

		, topLeftTexCoord(inLeft/imageWidth, inTop/imageHeight, 0.0f)
		, topRightTexCoord(inRight/imageWidth, inTop/imageHeight, 0.0f)
		, bottomRightTexCoord(inRight/imageWidth, inBottom/imageHeight, 0.0f)
		, bottomLeftTexCoord(inLeft/imageWidth, inBottom/imageHeight, 0.0f)
	{
	}

	float GetLetterWidth()
	{
		return width;
	}

	void GetLetterVerts(float* outVerts, float xOffset, float yOffset, float scale)
	{
		float scaledHeight = height*scale;
		float scaledWidth = width*scale;

		// triangle one

		// top left
		outVerts[0] = xOffset;
		outVerts[1] = yOffset;
		outVerts[2] = 0.0f;

		// bottom left
		outVerts[3] = xOffset;
		outVerts[4] = yOffset + scaledHeight;
		outVerts[5] = 0.0f;

		// bottom right
		outVerts[6] = xOffset + scaledWidth;
		outVerts[7] = yOffset + scaledHeight;
		outVerts[8] = 0.0f;

		// triangle two

		// top left
		outVerts[9] = xOffset;
		outVerts[10] = yOffset;
		outVerts[11] = 0.0f;

		// bottom right
		outVerts[12] = xOffset + scaledWidth;
		outVerts[13] = yOffset + scaledHeight;
		outVerts[14] = 0.0f;

		// top right
		outVerts[15] = xOffset + scaledWidth;
		outVerts[16] = yOffset;
		outVerts[17] = 0.0f;
	}

	void GetLetterTexCoords(float* outCoords)
	{
		// triangle one

		// top left
		outCoords[0] = topLeftTexCoord.x;
		outCoords[1] = topLeftTexCoord.y;

		// bottom left
		outCoords[2] = bottomLeftTexCoord.x;
		outCoords[3] = bottomLeftTexCoord.y;

		// bottom right
		outCoords[4] = bottomRightTexCoord.x;
		outCoords[5] = bottomRightTexCoord.y;

		// triangle two

		// top left
		outCoords[6] = topLeftTexCoord.x;
		outCoords[7] = topLeftTexCoord.y;

		// bottom right
		outCoords[8] = bottomRightTexCoord.x;
		outCoords[9] = bottomRightTexCoord.y;

		// top right
		outCoords[10] = topRightTexCoord.x;
		outCoords[11] = topRightTexCoord.y;
	}
};
const int NumDrawPosLetters = 256; 
DrawPos allLetters[NumDrawPosLetters]; 
MeshBuffer Text::textMesh;
int Text::textSheet = 0;
bool Text::hasTextMeshBeenInitialized = false;

const float LetterHeight = 47.0f;

const int NumLettersInMesh = 128;
const int NumTrianglesPerLetter = 2;
const int NumVertsPerTriangle = 3;
const int NumPositionElementsPerVert = 3;

const int NumPositionElementsPerLetter = NumTrianglesPerLetter * NumVertsPerTriangle * NumPositionElementsPerVert;
const int VertArrayLength = NumLettersInMesh * NumPositionElementsPerLetter;

const int NumTextCoordElementsPerVert = 2;
const int NumTexCoordElementsPerLetter = NumTrianglesPerLetter * NumVertsPerTriangle * NumTextCoordElementsPerVert;
const int TextCoordArrayLength = NumLettersInMesh * NumTexCoordElementsPerLetter;
//-----------------------------------

void InitLetters()
{	
	const DrawPos star(291, 27, 311, 27+LetterHeight);

	for (int letter = 0; letter < NumDrawPosLetters; ++letter)
	{
		allLetters[letter] = star; // all letters are initialized with a star character '*'
	}

	// row 1
	allLetters[(int)' '] = DrawPos(24, 27, 37, 27+LetterHeight);
	allLetters[(int)'!'] = DrawPos(46, 27, 58, 27+LetterHeight);
	allLetters[(int)'"'] = DrawPos(67, 27, 86, 27+LetterHeight);
	allLetters[(int)'#'] = DrawPos(95, 27, 118, 27+LetterHeight);
	allLetters[(int)'$'] = DrawPos(127, 27, 145, 27+LetterHeight);
	allLetters[(int)'%'] = DrawPos(154, 27, 187, 27+LetterHeight);
	allLetters[(int)'&'] = DrawPos(196, 27, 220, 27+LetterHeight);
	allLetters[(int)'\''] = DrawPos(229, 27, 239, 27+LetterHeight);
	allLetters[(int)'('] = DrawPos(248, 27, 261, 27+LetterHeight);
	allLetters[(int)')'] = DrawPos(270, 27, 282, 27+LetterHeight);
	//allLetters[(int)'*'] = 
	allLetters[(int)'+'] = DrawPos(320, 27, 348, 27+LetterHeight);
	allLetters[(int)','] = DrawPos(357, 27, 367, 27+LetterHeight);
	allLetters[(int)'-'] = DrawPos(376, 27, 389, 27+LetterHeight);
	allLetters[(int)'.'] = DrawPos(398, 27, 408, 27+LetterHeight);
	allLetters[(int)'/'] = DrawPos(417, 27, 438, 27+LetterHeight);

	// row 2
	allLetters[(int)'0'] = DrawPos(24, 83, 47, 83+LetterHeight);
	allLetters[(int)'1'] = DrawPos(56, 83, 68, 83+LetterHeight);
	allLetters[(int)'2'] = DrawPos(77, 83, 95, 83+LetterHeight);
	allLetters[(int)'3'] = DrawPos(104, 83, 123, 83+LetterHeight);
	allLetters[(int)'4'] = DrawPos(132, 83, 155, 83+LetterHeight);
	allLetters[(int)'5'] = DrawPos(164, 83, 185, 83+LetterHeight);
	allLetters[(int)'6'] = DrawPos(194, 83, 216, 83+LetterHeight);
	allLetters[(int)'7'] = DrawPos(225, 83, 246, 83+LetterHeight);
	allLetters[(int)'8'] = DrawPos(255, 83, 278, 83+LetterHeight);
	allLetters[(int)'9'] = DrawPos(287, 83, 309, 83+LetterHeight);
	allLetters[(int)':'] = DrawPos(318, 83, 328, 83+LetterHeight);
	allLetters[(int)';'] = DrawPos(337, 83, 347, 83+LetterHeight);
	allLetters[(int)'<'] = DrawPos(356, 83, 384, 83+LetterHeight);
	allLetters[(int)'='] = DrawPos(393, 83, 421, 83+LetterHeight);
	allLetters[(int)'>'] = DrawPos(430, 83, 458, 83+LetterHeight);
	allLetters[(int)'?'] = DrawPos(467, 83, 485, 83+LetterHeight);

	// row 3
	allLetters[(int)'@'] = DrawPos(24, 139, 54, 139+LetterHeight);
	allLetters[(int)'A'] = DrawPos(63, 139, 89, 139+LetterHeight);
	allLetters[(int)'B'] = DrawPos(98, 139, 121, 139+LetterHeight);
	allLetters[(int)'C'] = DrawPos(130, 139, 148, 139+LetterHeight);
	allLetters[(int)'D'] = DrawPos(157, 139, 180, 139+LetterHeight);
	allLetters[(int)'E'] = DrawPos(189, 139, 205, 139+LetterHeight);
	allLetters[(int)'F'] = DrawPos(214, 139, 230, 139+LetterHeight);
	allLetters[(int)'G'] = DrawPos(239, 139, 261, 139+LetterHeight);
	allLetters[(int)'H'] = DrawPos(270, 139, 293, 139+LetterHeight);
	allLetters[(int)'I'] = DrawPos(302, 139, 312, 139+LetterHeight);
	allLetters[(int)'J'] = DrawPos(321, 139, 343, 139+LetterHeight);
	allLetters[(int)'K'] = DrawPos(352, 139, 374, 139+LetterHeight);
	allLetters[(int)'L'] = DrawPos(383, 139, 399, 139+LetterHeight); 
	allLetters[(int)'M'] = DrawPos(408, 139, 441, 139+LetterHeight);
	allLetters[(int)'N'] = DrawPos(450, 139, 473, 139+LetterHeight);
	allLetters[(int)'O'] = DrawPos(482, 139, 505, 139+LetterHeight);

	// row 4
	allLetters[(int)'P'] = DrawPos(24, 195, 47, 195+LetterHeight);
	allLetters[(int)'Q'] = DrawPos(56, 195, 79, 195+LetterHeight);
	allLetters[(int)'R'] = DrawPos(88, 195, 113, 195+LetterHeight);
	allLetters[(int)'S'] = DrawPos(122, 195, 142, 195+LetterHeight);
	allLetters[(int)'T'] = DrawPos(151, 195, 172, 195+LetterHeight);
	allLetters[(int)'U'] = DrawPos(181, 195, 204, 195+LetterHeight);
	allLetters[(int)'V'] = DrawPos(213, 195, 239, 195+LetterHeight);
	allLetters[(int)'W'] = DrawPos(248, 195, 281, 195+LetterHeight);
	allLetters[(int)'X'] = DrawPos(290, 195, 313, 195+LetterHeight);
	allLetters[(int)'Y'] = DrawPos(322, 195, 350, 195+LetterHeight);
	allLetters[(int)'Z'] = DrawPos(359, 195, 378, 195+LetterHeight);
	allLetters[(int)'['] = DrawPos(387, 195, 402, 195+LetterHeight);
	allLetters[(int)'\\'] = DrawPos(411, 195, 433, 195+LetterHeight);
	allLetters[(int)']'] = DrawPos(442, 195, 457, 195+LetterHeight);
	//allLetters[(int)' '] = ;
	allLetters[(int)'_'] = DrawPos(488, 195, 509, 195+LetterHeight);

	// row 5
	allLetters[(int)'`'] = DrawPos(24, 251, 40, 251+LetterHeight);
	allLetters[(int)'a'] = DrawPos(49, 251, 71, 251+LetterHeight);
	allLetters[(int)'b'] = DrawPos(80, 251, 99, 251+LetterHeight);
	allLetters[(int)'c'] = DrawPos(108, 251, 123, 251+LetterHeight);
	allLetters[(int)'d'] = DrawPos(132, 251, 152, 251+LetterHeight);
	allLetters[(int)'e'] = DrawPos(161, 251, 175, 251+LetterHeight);
	allLetters[(int)'f'] = DrawPos(184, 251, 198, 251+LetterHeight);
	allLetters[(int)'g'] = DrawPos(207, 251, 226, 251+LetterHeight);
	allLetters[(int)'h'] = DrawPos(235, 251, 255, 251+LetterHeight);
	allLetters[(int)'i'] = DrawPos(264, 251, 273, 251+LetterHeight);
	allLetters[(int)'j'] = DrawPos(282, 251, 301, 251+LetterHeight);
	allLetters[(int)'k'] = DrawPos(310, 251, 329, 251+LetterHeight);
	allLetters[(int)'l'] = DrawPos(338, 251, 352, 251+LetterHeight);
	allLetters[(int)'m'] = DrawPos(361, 251, 389, 251+LetterHeight);
	allLetters[(int)'n'] = DrawPos(398, 251, 418, 251+LetterHeight);
	allLetters[(int)'o'] = DrawPos(427, 251, 447, 251+LetterHeight);

	// row 6
	allLetters[(int)'p'] = DrawPos(24, 307, 43, 307+LetterHeight);
	allLetters[(int)'q'] = DrawPos(52, 307, 72, 307+LetterHeight);
	allLetters[(int)'r'] = DrawPos(81, 307, 102, 307+LetterHeight);
	allLetters[(int)'s'] = DrawPos(111, 307, 128, 307+LetterHeight);
	allLetters[(int)'t'] = DrawPos(137, 307, 155, 307+LetterHeight);
	allLetters[(int)'u'] = DrawPos(164, 307, 184, 307+LetterHeight);
	allLetters[(int)'v'] = DrawPos(193, 307, 216, 307+LetterHeight);
	allLetters[(int)'w'] = DrawPos(225, 307, 253, 307+LetterHeight);
	allLetters[(int)'x'] = DrawPos(262, 307, 282, 307+LetterHeight);
	allLetters[(int)'y'] = DrawPos(291, 307, 315, 307+LetterHeight);
	allLetters[(int)'z'] = DrawPos(324, 307, 340, 307+LetterHeight);
	allLetters[(int)'{'] = DrawPos(349, 307, 366, 307+LetterHeight);
	//allLetters[(int)' '] = star;
	allLetters[(int)'}'] = DrawPos(397, 307, 414, 307+LetterHeight);
	//allLetters[(int)' '] = star;
	//allLetters[(int)' '] = star;
}

int LoadTexture()
{
	const int BufferSize = 256;
	char buffer[BufferSize];
#ifdef _WINDOWS
	sprintf_s(buffer, "%simages/spritefont32.bmp", GetDirectoryPath()); // create file name with path
#else
    sprintf(buffer, "%simages/spritefont32.bmp", GetDirectoryPath()); // create file name with path
#endif
	return TextureLoad(buffer, GL_FALSE//, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST, GL_CLAMP);
    , GL_LINEAR, GL_NEAREST, GL_CLAMP); //TODO TEXTURE FIX
}

void Text::CreateMesh()
{
	if (hasTextMeshBeenInitialized)
	{
		return;
	}
	hasTextMeshBeenInitialized = true;
	InitLetters();

	GLfloat vertexPositionArray[VertArrayLength];
	memset(vertexPositionArray, 0, sizeof(GLfloat)*VertArrayLength);
	
	textMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float), true);

	float vertexTexCoordsArray[TextCoordArrayLength];
	memset(vertexTexCoordsArray, 0, sizeof(float)*TextCoordArrayLength);
	
	textMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float), true);
	textMesh.SetTexture(textSheet);
}

void Text::DeleteMesh()
{
	if (hasTextMeshBeenInitialized)
	{
		hasTextMeshBeenInitialized = false;
		textMesh.Delete();
	}

	if (0 != textSheet)
	{
		deleteTexture(textSheet);
		textSheet = 0;
	}
}

Text::Text()
	: xPos(0.0f)
	, yPos(0.0f)
	, horizontalAlign(HorizontalAlignment::left)
	, virticalAlign(VirticalAlignment::top) 
	, size(LetterHeight)
{
	Init();
}

Text::Text(const char* text, float xPos, float yPos, HorizontalAlignment hAlign, VirticalAlignment vAlign, const CVector4& color, float size)
{
	Init();
	SetText(text, xPos, yPos, hAlign, vAlign, color, size);
}

// destructor
Text::~Text()
{
}

void Text::Init()
{
	currentText[0] = '\0';

	if (0 == textSheet)
	{
		textSheet = LoadTexture();
	}
	CreateMesh();
}

void Text::SetText(const char* text, float xPosIn, float yPosIn, HorizontalAlignment horizontalAlignIn, VirticalAlignment virticalAlignIn, 
				   const CVector4& colIn, float sizeIn)
{
	xPos = xPosIn;
	yPos = yPosIn;
	horizontalAlign = horizontalAlignIn;
	virticalAlign = virticalAlignIn; 
	col = colIn;
	SetSize(sizeIn);
	SetText(text);
}

void Text::SetText(const char* text, float xPosIn, float yPosIn, HorizontalAlignment horizontalAlignIn, VirticalAlignment virticalAlignIn, 
				   const CVector4& colIn)
{
	SetText(text, xPosIn, yPosIn, horizontalAlignIn, virticalAlignIn, colIn, size);
}

void Text::SetText(float xPosIn, float yPosIn, HorizontalAlignment horizontalAlignIn, VirticalAlignment virticalAlignIn, const CVector4& colIn)
{
	SetText(currentText, xPosIn, yPosIn, horizontalAlignIn, virticalAlignIn, colIn, size);
}

void Text::SetText(float theXPos, float theYPos)
{
	SetText(currentText, theXPos, theYPos, horizontalAlign, virticalAlign, col, size);
}

void Text::SetText(float xPosIn, float yPosIn, const char* string, ...)
{
	va_list vlist; // va_list
	
	va_start(vlist, string); // set to beginning of argument list
#ifdef _WINDOWS
	vsprintf_s
#else
    vsprintf
#endif
    (currentText, string, vlist); // print formatted string to thisString
	va_end(vlist); // reset argument pointer
	
	xPos = xPosIn;
	yPos = yPosIn;
}

void Text::SetSize(float sizeIn)
{
	size = sizeIn;
}

void Text::SetColor(const CVector4& color)
{
	col = color;
}

void Text::SetText(const char* string, ...)
{
	va_list vlist; // va_list
	
	va_start(vlist, string); // set to beginning of argument list
#ifdef _WINDOWS
    vsprintf_s
#else
    vsprintf
#endif
	(currentText, string, vlist); // print formatted string to thisString
	va_end(vlist); // reset argument pointer
}

void Text::GetTextBox(CVector& topLeft, CVector& bottomRight)
{
	topLeft = CVector(xPos, yPos, 0.0f);
	bottomRight = CVector(xPos, yPos, 0.0f);

	int numCharactersInString = (int)strlen(currentText);
	if (0 == numCharactersInString)
	{
		return;
	}

	float runningXPos = xPos;
	float scale = size/LetterHeight;
	for (int letter = 0; letter < numCharactersInString; ++letter)
	{
		char letterToDraw = currentText[letter];
		runningXPos += (allLetters[letterToDraw].GetLetterWidth()*scale);		
	}

	float stringWidth = runningXPos - xPos;

	topLeft.x = xPos;
	bottomRight.x = xPos+stringWidth;

	topLeft.y = yPos;
	bottomRight.y = yPos+(LetterHeight*scale);

	switch (horizontalAlign)
	{
	case HorizontalAlignment::center:
		topLeft.x -= (stringWidth/2.0f);
		bottomRight.x -= (stringWidth/2.0f);
		break;
	case HorizontalAlignment::right:		
		topLeft.x -= stringWidth;
		bottomRight.x -= stringWidth;
		break;
    default: break;
	}

	switch (virticalAlign)
	{
	case VirticalAlignment::middle:		
		topLeft.y -= ((LetterHeight*scale)/2.0f);
		bottomRight.y -= ((LetterHeight*scale)/2.0f);
		break;
	case VirticalAlignment::bottom:
		topLeft.y -= (LetterHeight*scale);
		bottomRight.y -= (LetterHeight*scale);
		break;
    default: break;
	}
}

float Text::GetTextHeight()
{
	CVector topLeft;
	CVector bottomRight;
	GetTextBox(topLeft, bottomRight);

	return bottomRight.y-topLeft.y;
}

float Text::GetTextWidth()
{
	CVector topLeft;
	CVector bottomRight;
	GetTextBox(topLeft, bottomRight);

	return bottomRight.x-topLeft.x;
}

void Text::Draw()
{
	int numCharactersInString = (int)strlen(currentText);
	if (0 == numCharactersInString)
	{
		return;
	}

	GLfloat vertexPositionArray[VertArrayLength];
	memset(vertexPositionArray, 0, sizeof(GLfloat)*VertArrayLength);

	float vertexTexCoordsArray[TextCoordArrayLength];
	memset(vertexTexCoordsArray, 0, sizeof(float)*TextCoordArrayLength);
	
	float runningXPos = xPos;
	float scale = size/LetterHeight;
	for (int letter = 0; letter < numCharactersInString; ++letter)
	{
		char letterToDraw = currentText[letter];

		allLetters[letterToDraw].GetLetterVerts(&(vertexPositionArray[letter * NumPositionElementsPerLetter]), runningXPos, yPos, scale);		
		allLetters[letterToDraw].GetLetterTexCoords(&(vertexTexCoordsArray[letter * NumTexCoordElementsPerLetter]));
		runningXPos += (allLetters[letterToDraw].GetLetterWidth()*scale);		
	}

	textMesh.UpdateVertexArray(vertexPositionArray);
	textMesh.SetColor(col);	
	textMesh.UpdateTexCoordArray(vertexTexCoordsArray);

	float stringWidth = runningXPos - xPos;
	CVector4 offset;
	
	switch (horizontalAlign)
	{
	case HorizontalAlignment::center:		
		offset.v3.x = -(stringWidth/2.0f);
		break;
	case HorizontalAlignment::right:		
		offset.v3.x = -stringWidth;
		break;
        default: break;
	}

	switch (virticalAlign)
	{
	case VirticalAlignment::middle:
		offset.v3.y = -((LetterHeight*scale)/2.0f);
		break;
	case VirticalAlignment::bottom:	
		offset.v3.y = -(LetterHeight*scale);
		break;
    default: break;
	}

	alignment.SetMatrixTranslation(offset);
	Globals::Instance().modelMatrixStack.PushMatrix();
	Globals::Instance().modelMatrixStack.MultMatrix(alignment);
		textMesh.Draw(numCharactersInString * NumTrianglesPerLetter * NumVertsPerTriangle );
	Globals::Instance().modelMatrixStack.PopMatrix();
}
#endif // #ifdef USE_SHADERS