// OpenGLOSX.h

#ifdef OSX

#ifndef _OpenGLOSX_H_
#define _OpenGLOSX_H_

//--- System Includes -----------
#include <Rendering/OpenGLInclude.h>
//-------------------------------

//--- Header files --------------
#include <Rendering/PlatformSpecific/OSX/OpenGLOSXShader.h>
//-------------------------------

//--- external variables --------
//-------------------------------

//--- forward declerations --------
class CMatrix;
class CVector;
class CVector4;
//-------------------------------

class OpenGLImplementation
{
	public:
		OpenGLImplementation();
		~OpenGLImplementation();

		bool InitGL();
		void DestroyGL();
   
        void ReSizeGLScene(int width, int height);
		void GLEnable(unsigned int identifier);
		void GLDisable(unsigned int identifier);

		void GLMaterial(unsigned int identifier, float value);
#ifdef _WINDOWS
		void GLMaterial(unsigned int identifier, float* valueArrayPointer);

		bool HasBeenInitialized();
		void UseProgram();
		void StopUsingProgram();
		int GetShaderProgram();
#endif // _WINDOWS
		int GetUniformLocation(const char* pUniformVariableName);
		unsigned int GetSubroutineUniformIndex(const char* pSubroutineUniformName, unsigned int shaderType);		
		int GetSubroutineIndex(const char* pSubRoutineName, unsigned int shaderType);
		int GetAttribLocation(const char* pUniformVariableName);
		void SetSubroutineUniformIndex(int subRoutineUniformIndex, unsigned int subRoutineIndex, unsigned int shaderType);
    
		void SetUniformFloat(int location, float newValue);
#ifdef _WINDOWS    
		void SetUniformBool(int location, bool newValue);
		void SetUniformInt(int location, int newValue);
		void SetUniformMatrix(int location, const CMatrix& mat);
		void SetUniformVector3(int location, const CVector& vect);
		void SetUniformVector4(int location, const CVector4& vect);

		template<typename T> unsigned int CreateVertexBufferObject(T* pArray, int numElements, unsigned int bufferType, bool isDynamic = false)
		{
			GLuint vertexBuffer = 0;
			glGenBuffers( 1, &vertexBuffer);
			glBindBuffer(bufferType, vertexBuffer);
			glBufferData(bufferType, numElements * sizeof( pArray[ 0]), pArray, (isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
			return vertexBuffer;
		}

		template<typename T> void UpdateVertexBufferObject(unsigned int vertexBuffer, T* pArray, int numElements, unsigned int bufferType)
		{
			glBindBuffer(bufferType, vertexBuffer);
			glBufferSubData(bufferType, 0, numElements * sizeof( pArray[ 0]), pArray);
		}

		void DeleteVertexBufferObject(unsigned int vertexBufferObject);

		void BindBuffer(unsigned int bufferType, int vertexBufferObject);
		void BufferData(float* vertices, int numElements);
		void VertexAttribPointer(int location, int numComponents, int stride = 0, int offset = 0);
		void DrawElements(int numToDraw);
		void DrawArrays(int numToDraw);

		void UseTextureDefault(int textureIndex);
		void UseTexture(int textureBindLayer, int textureIndex, int textureSamplerLoc);
		void DontUseDefaultTexture();

		void SetVertexPositionAttribPointer(int stride, int offset);
		void SetVertexNormalAttribPointer(int stride, int offset);
		void SetVertexTextureAttribPointer(int stride);

		void DisableVertexPositionAttribPointer();
		void DisableVertexNormalAttribPointer();
		void DisableVertexTextureAttribPointer();

		void SetProjectionMatrix(const CMatrix& mat);
		void SetModelMatrix(const CMatrix& mat);
		void SetViewMatrix(const CMatrix& mat);

		void SetAmbientColor(const CVector& color);
		void SetDiffuseColor(const CVector& color);
		void SetSpecularColor(const CVector& color);
		void SetEmissiveColor(const CVector4& color);
#endif
		void SetShininess(float val);
#ifdef _WINDOWS
		void ShowSpecularHighlights(bool show);

	private:
		bool AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);
		bool CompileShaders();

#endif // _WINDOWS
		OpenGLShader shader;

		unsigned int useTextureSubFunctionLocation;
		unsigned int noTextureSubFunctionLocation;

		int textureSamplerLocation;
		
		unsigned int useLightingSubFunctionLocation;
		unsigned int noLightingSubFunctionLocation;

		int projectionMatrixLocation;
		int modelMatrixLocation;
		int viewMatrixLocation;
		int vertexPositionAttribLocation;
		int vertexColorAttribLocation;
		int vertexNormalAttribLocation;
		int vertexTexCoordAttribLocation;
		int ambientColorLocation;
		int diffuseColorLocation;
		int specularColorLocation;
		int emissiveColorLocation;
		int shininessLocation;
		int showSpecularHighlightLocation;
		bool hasBeenInitialized;
		 
		int textureSubRoutineUniform;
		int colorSubRoutineUniform;
		int lightingSubRoutineUniform;
};

#endif

#endif