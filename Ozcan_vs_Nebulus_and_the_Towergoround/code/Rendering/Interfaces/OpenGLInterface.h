// OpenGLInterface.h

//system includes---
//------------------

#ifndef _OpenGLInterface_h_
#define _OpenGLInterface_h_

#ifdef _WINDOWS
#include "Rendering/PlatformSpecific/Windows/OpenGLWindows.h"
#elif OSX
#include "Rendering/PlatformSpecific/OSX/OpenGLOSX.h"
#else
#include "Rendering/PlatformSpecific/iOS/OpenGLiOS.h"
#endif

#include "Math\CMatrix.h"

class OpenGLInterface
{ 
	public:

		~OpenGLInterface()
		{

		}
    
		bool InitGL()
		{
			return implementation.InitGL();
		}

		void DestroyGL()
		{
			return implementation.DestroyGL();
		}

		void ReSizeGLScene(int width, int height)
		{
			implementation.ReSizeGLScene(width, height);
		}

		void GLEnable(unsigned int identifier)
		{
			implementation.GLEnable(identifier);
		}

		void GLDisable(unsigned int identifier)
		{
			implementation.GLDisable(identifier);			
		}

		void GLMaterial(unsigned int identifier, float value)
		{
			implementation.GLMaterial(identifier, value);
		}

		void GLMaterial(unsigned int identifier, float* valueArrayPointer)
		{
			implementation.GLMaterial(identifier, valueArrayPointer);
		}

		void ShowSpecularHighlights(bool show)
		{
			implementation.ShowSpecularHighlights(show);
		}

		bool HasBeenInitialized()
		{
			return implementation.HasBeenInitialized();
		}

		void UseProgram()
		{
			return implementation.UseProgram();
		}

		void StopUsingProgram()
		{
			return implementation.StopUsingProgram();
		}

		int GetShaderProgram()
		{
			return implementation.GetShaderProgram();
		}

		int GetUniformLocation(const char* pUniformVariableName)
		{
			return implementation.GetUniformLocation(pUniformVariableName);
		}

		unsigned int GetSubroutineUniformIndex(const char* pSubRoutineUniformName, unsigned int shaderType)
		{
			return implementation.GetSubroutineUniformIndex(pSubRoutineUniformName, shaderType);
		}

		void SetSubroutineUniformIndex(int subRoutineUniformIndex, unsigned int subRoutineIndex, unsigned int shaderType)
		{
			implementation.SetSubroutineUniformIndex(subRoutineUniformIndex, subRoutineIndex, shaderType);
		}

		int GetSubroutineIndex(const char* pSubRoutineName, unsigned int shaderType)
		{
			return implementation.GetSubroutineIndex(pSubRoutineName, shaderType);
		}

		int GetAttribLocation(const char* pUniformVariableName)
		{
			return implementation.GetAttribLocation(pUniformVariableName);
		}

		void UseTextureDefault(int textureIndex)
		{
			implementation.UseTextureDefault(textureIndex);
		}

		void UseTexture(int textureBindLayer, int textureIndex, int textureSamplerLoc)
		{
			implementation.UseTexture(textureBindLayer, textureIndex, textureSamplerLoc);
		}

		void DontUseDefaultTexture()
		{
			implementation.DontUseDefaultTexture();
		}

		void SetUniformFloat(int location, float newValue)
		{
			implementation.SetUniformFloat(location, newValue);
		}

		void SetUniformBool(int location, bool newValue)
		{
			implementation.SetUniformBool(location, newValue);
		}

		void SetUniformInt(int location, int newValue)
		{
			implementation.SetUniformInt(location, newValue);
		}

		void SetUniformMatrix(int location, const CMatrix& mat)
		{
			implementation.SetUniformMatrix(location, mat);
		}

		void SetUniformVector3(int location, const CVector& vect)
		{
			implementation.SetUniformVector3(location, vect);
		}

		void SetUniformVector4(int location, const CVector4& vect)
		{
			implementation.SetUniformVector4(location, vect);
		}

		template<typename T>unsigned int CreateVertexBufferObject(T* pArray, int numElements, unsigned int bufferType, bool isDynamic = false)
		{
			return implementation.CreateVertexBufferObject<T>(pArray, numElements, bufferType, isDynamic);
		}

		template<typename T> void UpdateVertexBufferObject(unsigned int vertexBuffer, T* pArray, int numElements, unsigned int bufferType)
		{
			implementation.UpdateVertexBufferObject(vertexBuffer, pArray, numElements, bufferType);
		}

		void DeleteVertexBufferObject(unsigned int vertexBufferObject)
		{
			return implementation.DeleteVertexBufferObject(vertexBufferObject);
		}

		void BindBuffer(unsigned int bufferType, int vertexBufferObject)
		{
			implementation.BindBuffer(bufferType, vertexBufferObject);
		}

		void VertexAttribPointer(int location, int numComponents)
		{
			implementation.VertexAttribPointer(location, numComponents);
		}

		void SetVertexPositionAttribPointer(int stride = 0, int offset = 0)
		{
			implementation.SetVertexPositionAttribPointer(stride, offset);
		}

		void SetVertexNormalAttribPointer(int stride = 0, int offset = 0)
		{
			implementation.SetVertexNormalAttribPointer(stride, offset);
		}

		void SetVertexTextureAttribPointer(int stride = 0)
		{
			implementation.SetVertexTextureAttribPointer(stride);
		}

		void DisableVertexPositionAttribPointer()
		{
			implementation.DisableVertexPositionAttribPointer();
		}

		void DisableVertexNormalAttribPointer()
		{
			implementation.DisableVertexNormalAttribPointer();
		}

		void DisableVertexTextureAttribPointer()
		{
			implementation.DisableVertexTextureAttribPointer();
		}

		void DrawElements(int numToDraw)
		{
			implementation.DrawElements(numToDraw);
		}

		void DrawArrays(int numToDraw)
		{
			implementation.DrawArrays(numToDraw);
		}

		void SetProjectionMatrix(const CMatrix& mat)
		{
			implementation.SetProjectionMatrix(mat);
		}

		void SetModelMatrix(const CMatrix& mat)
		{
			implementation.SetModelMatrix(mat);
		}

		void SetViewMatrix(const CMatrix& mat)
		{
			implementation.SetViewMatrix(mat);
		}

private:
	OpenGLImplementation implementation;
};

#endif // ifndef _CHighPrecisionTimerInterface_h_