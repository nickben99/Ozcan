//MatrixStack.cpp

// system includes --------
#include <Rendering/OpenGLInclude.h>
//-------------------------

// header files -----------
#include "MatrixStack.h"
#include "Game/Globals.h"
#ifndef USE_SHADERS
#include "Math/CMatrix.h"
#endif
//-------------------------

//--- external variables --------
//-------------------------------

//---static variable declerations ---
//-----------------------------------

// constructor
MatrixStack::MatrixStack()
#ifdef USE_SHADERS
	: stackCounter(0)
	, matrixLocation(-1)
#endif
{
}

// destructor
MatrixStack::~MatrixStack()
{
}

void MatrixStack::SetMatrixLocation(int loc)
{
	matrixLocation = loc;
}

void MatrixStack::LoadIdentity()
{
#ifdef USE_SHADERS
	stack[stackCounter].identity();
	Globals::Instance().gl.SetUniformMatrix(matrixLocation, stack[stackCounter]);
#else
	glLoadIdentity();
#endif
}

void MatrixStack::MultMatrix(const CMatrix& mat)
{
#ifdef USE_SHADERS
	stack[stackCounter] *= mat;
	Globals::Instance().gl.SetUniformMatrix(matrixLocation, stack[stackCounter]);
#else
	glMultMatrixf(&mat.elements[0]);
#endif
}

void MatrixStack::PushMatrix()
{
#ifdef USE_SHADERS
	assert(stackCounter < StackSize-1);
	if (stackCounter < StackSize-1)
	{
		stack[stackCounter + 1] = stack[stackCounter];
		++stackCounter;
	}
#else
	glPushMatrix();
#endif
}

void MatrixStack::PopMatrix()
{
#ifdef USE_SHADERS
	assert(stackCounter > 0);
	if (stackCounter > 0)
	{
		--stackCounter;
		Globals::Instance().gl.SetUniformMatrix(matrixLocation, stack[stackCounter]);
	}
#else
	glPopMatrix();
#endif
}