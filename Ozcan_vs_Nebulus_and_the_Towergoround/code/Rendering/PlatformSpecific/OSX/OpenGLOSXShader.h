// OpenGLOSXShader.h

#ifdef OSX

#ifdef USE_SHADERS

#ifndef _OpenGLOSXShader_H_
#define _OpenGLOSXShader_H_

//--- System Includes -----------
#include <Rendering/OpenGLInclude.h>
//-------------------------------

//--- Header files --------------
//-------------------------------

//--- external variables --------
//-------------------------------

//--- forward declerations -------
class CMatrix;
class CVector;
class CVector4;
//-------------------------------

#ifdef _DEBUG
#define CHECK_GL_ERROR OpenGLShader::CheckGLError(__FILE__, __LINE__)
#else
#define CHECK_GL_ERROR
#endif

class OpenGLShader
{
	public:
		OpenGLShader();
		~OpenGLShader();

		int GetShaderProgram();

		int GetUniformLocation(const char* pUniformVariableName);
		unsigned int GetSubroutineUniformIndex(const char* pSubroutineUniformVariableName, GLenum shaderType);
		int GetSubroutineIndex(const char* pSubRoutineName, unsigned int shaderType);
		int GetAttribLocation(const char* pUniformVariableName);

		void SetSubroutineUniformIndex(int subRoutineUniformIndex, unsigned int subRoutineIndex, GLenum shaderType, bool flushIfChanged, bool forceFlush);
		void SetUniformFloat(int location, float newValue);
		void SetUniformBool(int location, bool newValue);
		void SetUniformInt(int location, int newValue);
		void SetUniformMatrix(int location, const CMatrix& newValue);
		void SetUniformVector3(int location, const CVector& newValue);
		void SetUniformVector4(int location, const CVector4& newValue);

		bool CreateProgram(const char* vertexShader, const char* fragmentShader);
		void DestroyProgram();
		void UseProgram();

		void CacheSubroutineUniforms();

		static int CheckGLError(const char *file, int line);

	private:
		static bool AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType, GLuint& shaderObj);

		void CacheSubRoutineUniforms(unsigned int*& activeShaderSubRoutineUniformIndecis, char**& activeShaderSubRoutineUniformNames, 
									int& numActiveSubroutineUniforms, GLenum ShaderType);
		void DeleteSubRoutineCache();
		void DeleteSubRoutineCache(unsigned int*& activeShaderSubRoutineUniformIndecis, char**& activeShaderSubRoutineUniformNames, 
									int& numActiveSubroutineUniforms);

private:
	GLuint shaderProgram;
	GLuint compiledVertexShader;
	GLuint compiledFragmentShader;
	GLuint vao;

	int numActiveFragmentShaderSubroutineUniforms;
	unsigned int* activeFragmentShaderSubRoutineUniformIndecis;
	char** activeFragmentShaderSubRoutineUniformNames;

	int numActiveVertexShaderSubroutineUniforms;
	unsigned int* activeVertexShaderSubRoutineUniformIndecis;
	char** activeVertexShaderSubRoutineUniformNames;
//#endif // _WINDOWS
};

#endif // _OpenGLOSXShader_H_
#endif // USE_SHADERS
#endif // OSX