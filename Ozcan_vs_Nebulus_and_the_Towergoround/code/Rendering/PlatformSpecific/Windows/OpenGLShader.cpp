// OpenGLWindows.cpp - keymap implementation class

#ifdef _WINDOWS

#ifdef USE_SHADERS

//--- System includes ---------------
#include <windows.h>	// header file for windows
#include <glew-1.12.0\include\GL/glew.h>
#include <gl\glu.h>		// header file for the GLu32 library

#include <iostream>
#include <string>
//----------------------------------

//--- Header files ---------
#include "OpenGLShader.h" // the header file for this class
#include "FileReading\CTextFileReader.h"
#include "Game\defines.h"
#include "Math\CMatrix.h"
#include "Math\CVector.h"
//--------------------------

// GL ERROR CHECK
int OpenGLShader::CheckGLError(char *file, int line)
{
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR) 
    {
	    const GLubyte* sError = gluErrorString(glErr);

		char printOut[2048];				
		if (sError)
		{
			sprintf_s(printOut, "GL Error # %d (%s) in File %s at line: %d", glErr, sError, file, line);		
		}
		else
		{
			sprintf_s(printOut, "GL Error # %d (no message available) in File %s at line: %d", glErr, file, line);	
		}
		OutputDebugString(printOut);

		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

OpenGLShader::OpenGLShader()
	: shaderProgram(0)
	, compiledVertexShader(0)
	, compiledFragmentShader(0)
	, vao(0)
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
	unsigned int* activeShaderSubRoutineUniformIndecis = NULL;
	char** activeShaderSubRoutineUniformNames = NULL;
	int numActiveSubroutineUniforms = 0;

	if (GL_FRAGMENT_SHADER == shaderType)
	{
		activeShaderSubRoutineUniformIndecis = activeFragmentShaderSubRoutineUniformIndecis; 
		activeShaderSubRoutineUniformNames = activeFragmentShaderSubRoutineUniformNames;
		numActiveSubroutineUniforms = numActiveFragmentShaderSubroutineUniforms;
	}
	else if (GL_VERTEX_SHADER == shaderType)
	{
		activeShaderSubRoutineUniformIndecis = activeVertexShaderSubRoutineUniformIndecis; 
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

bool OpenGLShader::CreateProgram(const char* vertexShaderFileName, const char* fragmentShaderFileName)
{
	// Create Vertex Array Object
    glGenVertexArrays(1, &vao);
	CHECK_GL_ERROR;
    glBindVertexArray(vao);
	CHECK_GL_ERROR;

	shaderProgram = glCreateProgram();
	CHECK_GL_ERROR;
    if (shaderProgram == 0) 
	{
        return false;
    }
    
	char buffer[256];

    string vertexShader; 
	sprintf_s(buffer, "%scode/Shaders/%s", GetDirectoryPath(), vertexShaderFileName); // create file name with path
    if (!CTextFileReader::ReadFile(buffer, vertexShader)) 
	{
        return false;
    }

	string fragmentShader;
	sprintf_s(buffer, "%scode/Shaders/%s", GetDirectoryPath(), fragmentShaderFileName); // create file name with path
    if (!CTextFileReader::ReadFile(buffer, fragmentShader)) 
	{
        return false;
    }

	if (!AddShader(shaderProgram, vertexShader.c_str(), GL_VERTEX_SHADER, compiledVertexShader))
	{
		return false;
	}

	if (!AddShader(shaderProgram, fragmentShader.c_str(), GL_FRAGMENT_SHADER, compiledFragmentShader))
	{
		return false;
	}

    GLint Success = 0;
    //GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(shaderProgram);
	CHECK_GL_ERROR;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Success);
	CHECK_GL_ERROR;
	if (0 == Success) 
	{
		//glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		//fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
	}

    glValidateProgram(shaderProgram);
	CHECK_GL_ERROR;
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &Success);
	CHECK_GL_ERROR;
    if (!Success) 
	{
        //glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        //fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }
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
			glDeleteShader(compiledVertexShader);
			CHECK_GL_ERROR;
			compiledVertexShader = 0;
		}

		if (0 != compiledFragmentShader)
		{
			glDetachShader(shaderProgram, compiledFragmentShader);                 
			CHECK_GL_ERROR;
			glDeleteShader(compiledFragmentShader);
			CHECK_GL_ERROR;
			compiledFragmentShader = 0;
		}
	
		glDeleteProgram(shaderProgram);
		CHECK_GL_ERROR;
		shaderProgram = 0;
	}

	if (0 != vao)
	{
		glDeleteVertexArrays(1, &vao);
		CHECK_GL_ERROR;
		vao = 0;
	}
}

void OpenGLShader::UseProgram()
{
	glUseProgram(shaderProgram);
	CHECK_GL_ERROR;
}

bool OpenGLShader::AddShader(GLuint shaderProgram, const char* pShaderText, GLenum ShaderType, GLuint& shaderObj)
{
    shaderObj = glCreateShader(ShaderType);
	CHECK_GL_ERROR;
    if (shaderObj == 0) 
	{
        return false;
    }

    const GLchar* pShader[1];
    pShader[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);

    glShaderSource(shaderObj, 1, pShader, Lengths);
	CHECK_GL_ERROR;
    glCompileShader(shaderObj);
	CHECK_GL_ERROR;

    GLint success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	CHECK_GL_ERROR;
    if (!success) 
	{
#ifdef _DEBUG
        GLchar InfoLog[1024];
        glGetShaderInfoLog(shaderObj, 1024, NULL, InfoLog);
        
		char printOut[2048];
		sprintf_s(printOut, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);		
		OutputDebugString(printOut);
#endif
        return false;
    }

    glAttachShader(shaderProgram, shaderObj);
	CHECK_GL_ERROR;
	return true;
}

#endif // USE_SHADERS

#endif // _WINDOWS