// OpenGLShader.cpp
#ifdef USE_SHADERS

//--- System includes ---------------
#ifdef _WINDOWS
#include <windows.h>	// header file for windows
#include <glew-1.12.0\include\GL/glew.h>
#include <gl\glu.h>		// header file for the GLu32 library
#elif OSX
#include <Rendering/OpenGLInclude.h>
#endif

#include <iostream>
#include <string>
#include <vector>

#ifdef OSX
#include <stdio.h> // for sprintf
#endif
//----------------------------------

//--- Header files ---------
#include "OpenGLShader.h" // the header file for this class
#include "FileReading/CTextFileReader.h"
#include "Game/defines.h"
#include "Math/CMatrix.h"
#include "Math/CVector.h"
#include <System/Interfaces/SystemInterface.h>
//--------------------------

// GL ERROR CHECK
int OpenGLShader::CheckGLError(const char *file, int line)
{
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR) 
    {
        const GLubyte* sError = nullptr;
#ifdef _WINDOWS
	    sError = gluErrorString(glErr);
#endif

        const char* sErrorCode = nullptr;
        switch (glErr)
        {
            case GL_INVALID_ENUM: sErrorCode = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: sErrorCode = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: sErrorCode = "GL_INVALID_OPERATION"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: sErrorCode = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            case GL_OUT_OF_MEMORY: sErrorCode = "GL_OUT_OF_MEMORY"; break;
            case GL_STACK_UNDERFLOW: sErrorCode = "GL_STACK_UNDERFLOW"; break;
            case GL_STACK_OVERFLOW: sErrorCode = "GL_STACK_OVERFLOW"; break;
            default: sErrorCode = "(no message available)"; break;
        }
			
		if (sError)
		{
            ToConsole("\nGL Error # %s (%s) in File %s at line: %d", sErrorCode, sError, file, line);
		}
		else
		{
            ToConsole("\nGL Error # %s in File %s at line: %d", sErrorCode, file, line);
		}
        
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

OpenGLShader::OpenGLShader()
	: shaderProgram(0)
	, compiledVertexShader(0)
	, compiledFragmentShader(0)
	//, vao(0)
	, numActiveFragmentShaderSubroutineUniforms(0)
	, activeFragmentShaderSubRoutineUniformIndecis(NULL)
	, activeFragmentShaderSubRoutineUniformNames(NULL)
	, numActiveVertexShaderSubroutineUniforms(0)
	, activeVertexShaderSubRoutineUniformIndecis(NULL)
	, activeVertexShaderSubRoutineUniformNames(NULL)
{
}

OpenGLShader::~OpenGLShader()
{	
	DestroyProgram();
}

int OpenGLShader::GetShaderProgram()
{
	return shaderProgram;
}

int OpenGLShader::GetUniformLocation(const char* pUniformVariableName)
{
	int res = glGetUniformLocation(shaderProgram, pUniformVariableName);
	CHECK_GL_ERROR;
	return res;
}

void OpenGLShader::SetSubroutineUniformIndex(int subRoutineUniformIndex, unsigned int subRoutineIndex, GLenum shaderType, bool flushIfChanged, bool forceFlush)
{	
	unsigned int prevValue = 0;
	if (GL_FRAGMENT_SHADER == shaderType)
	{
		prevValue = activeFragmentShaderSubRoutineUniformIndecis[subRoutineUniformIndex];
		activeFragmentShaderSubRoutineUniformIndecis[subRoutineUniformIndex] = subRoutineIndex;
	}
	else if (GL_VERTEX_SHADER == shaderType)
	{
		prevValue = activeVertexShaderSubRoutineUniformIndecis[subRoutineUniformIndex];
		activeVertexShaderSubRoutineUniformIndecis[subRoutineUniformIndex] = subRoutineIndex;
	}

	if (forceFlush || 
		(flushIfChanged && prevValue != subRoutineIndex))
	{
		if (GL_FRAGMENT_SHADER == shaderType)
		{
			glUniformSubroutinesuiv(shaderType, numActiveFragmentShaderSubroutineUniforms, activeFragmentShaderSubRoutineUniformIndecis);			
		}
		else if (GL_VERTEX_SHADER == shaderType)
		{
			glUniformSubroutinesuiv(shaderType, numActiveVertexShaderSubroutineUniforms, activeVertexShaderSubRoutineUniformIndecis);
		}
	}
}

unsigned int OpenGLShader::GetSubroutineUniformIndex(const char* pSubroutineUniformVariableName, GLenum shaderType)
{
	char** activeShaderSubRoutineUniformNames = NULL;
	int numActiveSubroutineUniforms = 0;

	if (GL_FRAGMENT_SHADER == shaderType)
	{
		activeShaderSubRoutineUniformNames = activeFragmentShaderSubRoutineUniformNames;
		numActiveSubroutineUniforms = numActiveFragmentShaderSubroutineUniforms;
	}
	else if (GL_VERTEX_SHADER == shaderType)
	{
		activeShaderSubRoutineUniformNames = activeVertexShaderSubRoutineUniformNames;
		numActiveSubroutineUniforms = numActiveVertexShaderSubroutineUniforms;
	}
	else
	{
		return 0;
	}

	for (int uniformSubroutine = 0; uniformSubroutine < numActiveSubroutineUniforms; ++uniformSubroutine)
	{
		if (strstr(activeShaderSubRoutineUniformNames[uniformSubroutine], pSubroutineUniformVariableName))
		{
			return uniformSubroutine;		
		}
	}
	return 0;
}

void OpenGLShader::DeleteSubRoutineCache()
{
	DeleteSubRoutineCache(activeVertexShaderSubRoutineUniformIndecis, activeVertexShaderSubRoutineUniformNames, numActiveVertexShaderSubroutineUniforms);
	DeleteSubRoutineCache(activeFragmentShaderSubRoutineUniformIndecis, activeFragmentShaderSubRoutineUniformNames, numActiveFragmentShaderSubroutineUniforms);
}

void OpenGLShader::DeleteSubRoutineCache(unsigned int*& activeShaderSubRoutineUniformIndecis, char**& activeShaderSubRoutineUniformNames, 
										   int& numActiveSubroutineUniforms)
{
	if (NULL != activeShaderSubRoutineUniformNames)
	{
		for (int subroutineUniform = 0; subroutineUniform < numActiveSubroutineUniforms; ++subroutineUniform)
		{
			if (NULL != activeShaderSubRoutineUniformNames[subroutineUniform])
			{
				delete[] activeShaderSubRoutineUniformNames[subroutineUniform];
				activeShaderSubRoutineUniformNames[subroutineUniform] = NULL;
			}
		}
		delete[] activeShaderSubRoutineUniformNames;
		activeShaderSubRoutineUniformNames = NULL;
	}

	numActiveSubroutineUniforms = 0;
	if (NULL != activeShaderSubRoutineUniformIndecis)
	{
		delete[] activeShaderSubRoutineUniformIndecis;
		activeShaderSubRoutineUniformIndecis = NULL;
	}
}

void OpenGLShader::CacheSubroutineUniforms()
{
	DeleteSubRoutineCache();

	CacheSubRoutineUniforms(activeVertexShaderSubRoutineUniformIndecis, activeVertexShaderSubRoutineUniformNames, numActiveVertexShaderSubroutineUniforms, GL_VERTEX_SHADER);
	CacheSubRoutineUniforms(activeFragmentShaderSubRoutineUniformIndecis, activeFragmentShaderSubRoutineUniformNames, numActiveFragmentShaderSubroutineUniforms, GL_FRAGMENT_SHADER);
}

void OpenGLShader::CacheSubRoutineUniforms(unsigned int*& activeShaderSubRoutineUniformIndecis, char**& activeShaderSubRoutineUniformNames, 
										   int& numActiveSubroutineUniforms, GLenum shaderType)
{
	glGetProgramStageiv(GetShaderProgram(), shaderType, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numActiveSubroutineUniforms);
	CHECK_GL_ERROR;

	if (0 == numActiveSubroutineUniforms)
	{
		return;
	}

	activeShaderSubRoutineUniformIndecis = new unsigned int[numActiveSubroutineUniforms];
	activeShaderSubRoutineUniformNames = new char*[numActiveSubroutineUniforms];

	for (int subroutineUniform = 0; subroutineUniform < numActiveSubroutineUniforms; ++subroutineUniform)
	{
		const int NameBufferSize = 1024;
		activeShaderSubRoutineUniformNames[subroutineUniform] = new char[NameBufferSize];
		GLsizei subroutineNameLength = 0;
		glGetActiveSubroutineUniformName(GetShaderProgram(), shaderType, subroutineUniform, NameBufferSize, &subroutineNameLength, activeShaderSubRoutineUniformNames[subroutineUniform]);
		CHECK_GL_ERROR;

		int numCompatibleSubRoutines = 0;		
		glGetActiveSubroutineUniformiv(GetShaderProgram(), GL_FRAGMENT_SHADER, subroutineUniform, GL_NUM_COMPATIBLE_SUBROUTINES, &numCompatibleSubRoutines);
		CHECK_GL_ERROR;
		if (numCompatibleSubRoutines > 0)
		{
			int* compatibleSubRoutines = new int[numCompatibleSubRoutines];		
			glGetActiveSubroutineUniformiv(GetShaderProgram(), GL_FRAGMENT_SHADER, subroutineUniform, GL_COMPATIBLE_SUBROUTINES, compatibleSubRoutines);
			CHECK_GL_ERROR;
			activeShaderSubRoutineUniformIndecis[subroutineUniform] = (unsigned int)compatibleSubRoutines[0]; // set to any default sub routine
			delete[] compatibleSubRoutines;
		}
	}
}

int OpenGLShader::GetSubroutineIndex(const char* pSubRoutineName, unsigned int shaderType)
{
	int index = glGetSubroutineIndex(GetShaderProgram(), shaderType, pSubRoutineName);
	CHECK_GL_ERROR;
	return index;
}

int OpenGLShader::GetAttribLocation(const char* pUniformVariableName)
{
	int res = glGetAttribLocation(shaderProgram, pUniformVariableName);
	CHECK_GL_ERROR;
	return res;
}

void OpenGLShader::SetUniformFloat(int location, float newValue)
{
	glUniform1f(location, newValue);
	CHECK_GL_ERROR;
}

void OpenGLShader::SetUniformBool(int location, bool newValue)
{
	CHECK_GL_ERROR;
	glUniform1i(location, newValue ? 1 : 0);
	CHECK_GL_ERROR;
}

void OpenGLShader::SetUniformInt(int location, int newValue)
{
	glUniform1i(location, newValue);
	CHECK_GL_ERROR;
}

void OpenGLShader::SetUniformMatrix(int location, const CMatrix& newValue)
{
	CHECK_GL_ERROR;
	const int numMatrices = 1;
	const unsigned char isTranspose = 0;	
	const float* data = newValue.elements;
	glUniformMatrix4fv(location, numMatrices, isTranspose, data);
	CHECK_GL_ERROR;
}

void OpenGLShader::SetUniformVector3(int location, const CVector& newValue)
{
	glUniform3f(location, newValue.x, newValue.y, newValue.z);
	CHECK_GL_ERROR;
}

void OpenGLShader::SetUniformVector4(int location, const CVector4& newValue)
{
	glUniform4f(location, newValue.v3.x, newValue.v3.y, newValue.v3.z, newValue.w);
	CHECK_GL_ERROR;
}

void PrintShaderInfoLog(GLuint object, const char* format) {
	int InfoLogLength = 0;
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> message(InfoLogLength + 1);
		glGetShaderInfoLog(object, InfoLogLength, NULL, &message[0]);
		ToConsole(format, &message[0]);
	}
}

void PrintProgramInfoLog(GLuint object, const char* format) {
	int InfoLogLength = 0;
	glGetProgramiv(object, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> message(InfoLogLength + 1);
		glGetProgramInfoLog(object, InfoLogLength, NULL, &message[0]);
		ToConsole(format, &message[0]);
	}
}

bool OpenGLShader::CreateProgram(const char* vertexShaderFileName, const char* fragmentShaderFileName)
{
	// Create Vertex Array Object
//    glGenVertexArrays(1, &vao);
//	CHECK_GL_ERROR;
//    glBindVertexArray(vao);
//	CHECK_GL_ERROR;

    // Create the shaders
    compiledVertexShader = glCreateShader(GL_VERTEX_SHADER);
    compiledFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	char buffer[256];
	std::string vertexShaderCode;
	SPRINTF(buffer, "%scode/Shaders/%s", GetDirectoryPath(), vertexShaderFileName); // create file name with path
	if (!CTextFileReader::ReadFile(buffer, vertexShaderCode)) {
		ToConsole("\nVertex shader file could not be opened");
		return false;
	}

	string fragmentShaderCode;
	SPRINTF(buffer, "%scode/Shaders/%s", GetDirectoryPath(), fragmentShaderFileName); // create file name with path
	if (!CTextFileReader::ReadFile(buffer, fragmentShaderCode)) {
		ToConsole("\nFragment shader file could not be opened");
		return false;
	}
    
    GLint Result = GL_FALSE;

	ToConsole("\nCompiling vertex shader : %s\n", vertexShaderFileName);
	if (!AddShader(vertexShaderCode.c_str(), compiledVertexShader)) {
		ToConsole("\n\nVertex shader did not compile");
		return false;
	}

	ToConsole("\nCompiling fragment shader : %s\n", fragmentShaderFileName);
	if (!AddShader(fragmentShaderCode.c_str(), compiledFragmentShader)) {
		ToConsole("\n\nVertex shader did not compile");
		return false;
	}

    // Link the program
    ToConsole("\nLinking program\n");
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, compiledVertexShader);
    glAttachShader(shaderProgram, compiledFragmentShader);
    glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Result);
	PrintProgramInfoLog(shaderProgram, "%s\n");
	if (GL_FALSE == Result) {
		ToConsole("\n\nShader did not link");
		return false;
	}

	ToConsole("\nValidating program\n");
	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &Result);
	PrintProgramInfoLog(shaderProgram, "%s\n");
	if (GL_FALSE == Result) { // shader not validating should not result in returning false as uniforms are not set yet
		ToConsole("\n\nShader did not validate");
	}

    glDetachShader(shaderProgram, compiledVertexShader);
    glDetachShader(shaderProgram, compiledFragmentShader);
    
    glDeleteShader(compiledVertexShader);
    glDeleteShader(compiledFragmentShader);
    
    compiledVertexShader = 0;
    compiledFragmentShader = 0;
    
    return true;
}

void OpenGLShader::DestroyProgram()
{
	DeleteSubRoutineCache();

	if (0 != shaderProgram)
	{
		if (0 != compiledVertexShader)
		{
			glDetachShader(shaderProgram, compiledVertexShader);                 
			CHECK_GL_ERROR;
		}

		if (0 != compiledFragmentShader)
		{
			glDetachShader(shaderProgram, compiledFragmentShader);                 
			CHECK_GL_ERROR;
		}
	}

    if (0 != compiledVertexShader)
    {
        glDeleteShader(compiledVertexShader);
        CHECK_GL_ERROR;
        compiledVertexShader = 0;
    }
    
    if (0 != compiledFragmentShader)
    {
        glDeleteShader(compiledFragmentShader);
        CHECK_GL_ERROR;
        compiledFragmentShader = 0;
    }
    
    if (0 != shaderProgram)
    {
        glDeleteProgram(shaderProgram);
        CHECK_GL_ERROR;
        shaderProgram = 0;
    }
    
//	if (0 != vao)
//	{
//		glDeleteVertexArrays(1, &vao);
//		CHECK_GL_ERROR;
//		vao = 0;
//	}
}

void OpenGLShader::UseProgram()
{
	glUseProgram(shaderProgram);
	CHECK_GL_ERROR;
}

bool OpenGLShader::AddShader(const char* pShaderText, GLuint shaderObj)
{
	glShaderSource(shaderObj, 1, &pShaderText, NULL);
	glCompileShader(shaderObj);

	GLint Result = GL_FALSE;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &Result);
	PrintShaderInfoLog(shaderObj, "\n\nError compiling shader: \n%s\n");
	return GL_TRUE == Result;
}

#endif // USE_SHADERS