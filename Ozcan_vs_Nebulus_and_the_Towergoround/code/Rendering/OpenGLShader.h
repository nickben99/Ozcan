// OpenGLShader.h

#ifdef USE_SHADERS

#ifndef _OpenGLShader_H_
#define _OpenGLShader_H_

//--- System Includes -----------
#ifdef OSX
#include <Rendering/OpenGLInclude.h>
#endif
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
		static void SetUniformFloat(int location, float newValue);
		static void SetUniformBool(int location, bool newValue);
		static void SetUniformInt(int location, int newValue);
		static void SetUniformMatrix(int location, const CMatrix& newValue);
		static void SetUniformVector3(int location, const CVector& newValue);
		static void SetUniformVector4(int location, const CVector4& newValue);

		bool CreateProgram(const char* vertexShader, const char* fragmentShader);
		void DestroyProgram();
		void UseProgram();

		void CacheSubroutineUniforms();

		static int CheckGLError(const char *file, int line);

	private:
		void CacheSubRoutineUniforms(unsigned int*& activeShaderSubRoutineUniformIndecis, char**& activeShaderSubRoutineUniformNames, 
									int& numActiveSubroutineUniforms, GLenum ShaderType);
		void DeleteSubRoutineCache();
		void DeleteSubRoutineCache(unsigned int*& activeShaderSubRoutineUniformIndecis, char**& activeShaderSubRoutineUniformNames, 
									int& numActiveSubroutineUniforms);

private:
	GLuint shaderProgram;
	GLuint compiledVertexShader;
	GLuint compiledFragmentShader;
	//GLuint vao;

	int numActiveFragmentShaderSubroutineUniforms;
	unsigned int* activeFragmentShaderSubRoutineUniformIndecis;
	char** activeFragmentShaderSubRoutineUniformNames;

	int numActiveVertexShaderSubroutineUniforms;
	unsigned int* activeVertexShaderSubRoutineUniformIndecis;
	char** activeVertexShaderSubRoutineUniformNames;
};

#endif // _OpenGLShader_H_
#endif // USE_SHADERS