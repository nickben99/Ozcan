//MatrixStack.h

//system includes------------
//---------------------------

#ifndef _MatrixStack_h_
#define _MatrixStack_h_

//header files---------
#ifdef USE_SHADERS
#include "Math/CMatrix.h"
#endif
//---------------------

//defines--------------

//---------------------

//forward declerations----
#ifndef USE_SHADERS
class CMatrix;
#endif
//------------------------

class MatrixStack
{
public:
	MatrixStack();
	~MatrixStack();

	void SetMatrixLocation(int loc);

	void LoadIdentity();
	void MultMatrix(const CMatrix& mat);
	void PushMatrix();
	void PopMatrix();

private:
#ifdef USE_SHADERS
	static const int StackSize = 10;
	CMatrix stack[StackSize];
	int stackCounter;

	int matrixLocation;
#endif
};

#endif // ifndef _MatrixStack_h_ 