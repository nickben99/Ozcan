#include "CTest.h"
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>

//
//// Include GLEW
#include <GL/glew.h>
#include </usr/local/Cellar/glfw3/3.1.2/include/GLFW/glfw3.h>

//// Include GLM
#include </Users/benjaminnickson/Documents/for stack overflow/Ozcan-master/Ozcan_vs_Nebulus_and_the_Towergoround/external/glm-0.9.7.1/glm/glm.hpp>
#include </Users/benjaminnickson/Documents/for stack overflow/Ozcan-master/Ozcan_vs_Nebulus_and_the_Towergoround/external/glm-0.9.7.1/glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <Math/CVector.h>
#include <Math/CMatrix.h>
#include <Game/Globals.h>

// GL ERROR CHECK
int CheckGLError(const char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;
    
    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        const GLubyte* sError = nullptr;
        
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
        char printOut[2048];
        if (sError)
        {
            sprintf(printOut, "\nGL Error # %s (%s) in File %s at line: %d", sErrorCode, sError, file, line);
        }
        else
        {
            sprintf(printOut, "\nGL Error # %s in File %s at line: %d", sErrorCode, file, line);
        }
        std::cout << printOut;
        
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

#define CHECK_GL_ERROR CheckGLError(__FILE__, __LINE__)

// ---------
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open()){
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }
    
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
    
    
    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    
    
    
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }
    
    
    
    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }
    
    
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath){
    
    unsigned char header[124];
    
    FILE *fp;
    
    /* try to open the file */
    fp = fopen(imagepath, "rb");
    if (fp == NULL){
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
        return 0;
    }
    
    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        return 0;
    }
    
    /* get the surface desc */
    fread(&header, 124, 1, fp);
    
    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width	     = *(unsigned int*)&(header[12]);
    unsigned int linearSize	 = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);
    
    
    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);
    
    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            free(buffer);
            return 0;
    }
    
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;
    
    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                               0, size, buffer + offset);
        CHECK_GL_ERROR;
        
        offset += size;
        width  /= 2;
        height /= 2;
        
        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if(width < 1) width = 1;
        if(height < 1) height = 1;
        
    }
    
    free(buffer);
    
    return textureID;
    
    
}

bool loadOBJ(
             const char * path,
             std::vector<CVector> & out_vertices,
             std::vector<CVector2> & out_uvs,
             std::vector<CVector> & out_normals
             ){
    printf("Loading OBJ file %s...\n", path);
    
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<CVector> temp_vertices;
    std::vector<CVector2> temp_uvs;
    std::vector<CVector> temp_normals;
    
    
    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
        getchar();
        return false;
    }
    
    while( 1 ){
        
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        
        // else : parse lineHeader
        
        if ( strcmp( lineHeader, "v" ) == 0 ){
            CVector vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);
        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            CVector2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            CVector normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
        
    }
    
    // For each vertex of each triangle
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){
        
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];
        
        // Get the attributes thanks to the index
        CVector vertex = temp_vertices[ vertexIndex-1 ];
        CVector2 uv = temp_uvs[ uvIndex-1 ];
        CVector normal = temp_normals[ normalIndex-1 ];
        
        // Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs     .push_back(uv);
        out_normals .push_back(normal);
        
    }
    
    return true;
}

struct PackedVertex{
    CVector position;
    CVector2 uv;
    CVector normal;
    bool operator<(const PackedVertex that) const{
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
    };
};

bool getSimilarVertexIndex_fast(
                                PackedVertex & packed,
                                std::map<PackedVertex,unsigned short> & VertexToOutIndex,
                                unsigned short & result
                                ){
    std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
    if ( it == VertexToOutIndex.end() ){
        return false;
    }else{
        result = it->second;
        return true;
    }
}

void indexVBO(
              std::vector<CVector> & in_vertices,
              std::vector<CVector2> & in_uvs,
              std::vector<CVector> & in_normals,
              
              std::vector<unsigned short> & out_indices,
              std::vector<CVector> & out_vertices,
              std::vector<CVector2> & out_uvs,
              std::vector<CVector> & out_normals
              ){
    std::map<PackedVertex,unsigned short> VertexToOutIndex;
    
    // For each input vertex
    for ( unsigned int i=0; i<in_vertices.size(); i++ ){
        
        PackedVertex packed = {in_vertices[i], in_uvs[i], in_normals[i]};
        
        
        // Try to find a similar vertex in out_XXXX
        unsigned short index;
        bool found = getSimilarVertexIndex_fast( packed, VertexToOutIndex, index);
        
        if ( found ){ // A similar vertex is already in the VBO, use it instead !
            out_indices.push_back( index );
        }else{ // If not, it needs to be added in the output data.
            out_vertices.push_back( in_vertices[i]);
            out_uvs     .push_back( in_uvs[i]);
            out_normals .push_back( in_normals[i]);
            unsigned short newindex = (unsigned short)out_vertices.size() - 1;
            out_indices .push_back( newindex );
            VertexToOutIndex[ packed ] = newindex;
        }
    }
}
// ---------

GLuint Texture = 0;
int windowWidth = 1024;
int windowHeight = 768;
std::vector<CVector> vertices;
std::vector<CVector2> uvs;
std::vector<CVector> normals;
std::vector<unsigned short> indices;
std::vector<CVector> indexed_vertices;
std::vector<CVector2> indexed_uvs;
std::vector<CVector> indexed_normals;
GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;
GLuint elementbuffer;
const int textureSize = 1024;
CVector lightInvDir(0.5f,2,2);
glm::vec3 lightInvDirGLM(0.5f,2,2);
GLuint VertexArrayID;

struct Original {
    // depth shader
    GLuint depthProgramID = 0;
    GLuint depthMatrixID = 0;
    
    // shadow shader
    GLuint programID = 0;
    GLuint TextureID = 0;
    GLuint MatrixID = 0;
    GLuint ViewMatrixID = 0;
    GLuint ModelMatrixID = 0;
    GLuint DepthBiasID = 0;
    GLuint ShadowMapID = 0;
    GLuint lightInvDirID = 0;
    
    // shadow mapping init
    GLuint FramebufferName = 0;
    GLuint depthTexture = 0;
    
    // matrices/vectors
//    CMatrix depthProjectionMatrix;
//    CMatrix depthViewMatrix;
//    CMatrix depthModelMatrix;
//    CMatrix depthMVP;
    
    glm::mat4 depthProjectionMatrix;
    glm::mat4 depthViewMatrix;
    glm::mat4 depthModelMatrix;
    glm::mat4 depthMVP;
} originalInst;

struct Ozcan {
    GLuint theOnlyProgramID = 0;
    
    // uniforms
    GLuint textureSamplerLocation = 0;
    
    GLuint projectionMatrixLocation = 0;
    GLuint modelMatrixLocation = 0;
    GLuint viewMatrixLocation = 0;
    
    GLuint ambientColorLocation = 0;
    GLuint diffuseColorLocation = 0;
    GLuint specularColorLocation = 0;
    GLuint emissiveColorLocation = 0;
    GLuint shininessLocation = 0;
    
    GLuint oldCodeTextureSelection = 0;
    GLuint oldCodeLightingSelection = 0;
    
    // uniforms for rendering depth texture
    GLuint oldCodeVertexRenderSelector = 0;
    GLuint oldCodeFragmentRenderSelector = 0;
    
    GLuint lightViewProjectionMatrixUniform = 0;
    GLuint depthTextureSamplerUniform = 0;
    
    // lighting uniforms
    GLuint pointLightingLocationUniform = 0;
    
    // shadow mapping init
    GLuint framebufferName = 0;
    GLuint depthTexture = 0;
} ozcanInst;

void OriginalLoadShaders() {
    originalInst.depthProgramID = LoadShaders( "DepthRTTVert.glsl", "DepthRTTFrag.glsl" );
    CHECK_GL_ERROR;
    
    // Create and compile our GLSL program from the shaders
    originalInst.programID = LoadShaders( "ShadowMappingVert.glsl", "ShadowMappingFrag.glsl" );
    CHECK_GL_ERROR;
}

void OzcanLoadShaders() {
    ozcanInst.theOnlyProgramID = LoadShaders( "code/Shaders/ShaderOld.vs", "code/Shaders/ShaderOld.fs" );
    CHECK_GL_ERROR;
    // Use our shader
    glUseProgram(ozcanInst.theOnlyProgramID);
    CHECK_GL_ERROR;
}

void OriginalGetUniformLocation() {
    // Get a handle for our "MVP" uniform
    originalInst.depthMatrixID = glGetUniformLocation(originalInst.depthProgramID, "depthMVP");
    CHECK_GL_ERROR;
    
    // shadow
    // Get a handle for our "myTextureSampler" uniform
    originalInst.TextureID  = glGetUniformLocation(originalInst.programID, "myTextureSampler");
    
    // Get a handle for our "MVP" uniform
    originalInst.MatrixID = glGetUniformLocation(originalInst.programID, "MVP");
    originalInst.ViewMatrixID = glGetUniformLocation(originalInst.programID, "V");
    originalInst.ModelMatrixID = glGetUniformLocation(originalInst.programID, "M");
    originalInst.DepthBiasID = glGetUniformLocation(originalInst.programID, "DepthBiasMVP");
    originalInst.ShadowMapID = glGetUniformLocation(originalInst.programID, "shadowMap");
    
    // Get a handle for our "LightPosition" uniform
    originalInst.lightInvDirID = glGetUniformLocation(originalInst.programID, "LightInvDirection_worldspace");
}

void OzcanGetUniformLocation() {
    ozcanInst.textureSamplerLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uSampler"); //////
    
    ozcanInst.projectionMatrixLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uProjectionMatrix");
    ozcanInst.modelMatrixLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uModelMatrix");
    ozcanInst.viewMatrixLocation =  glGetUniformLocation(ozcanInst.theOnlyProgramID, "uViewMatrix");
    
    ozcanInst.ambientColorLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uAmbientColor");
    ozcanInst.diffuseColorLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uPointLightingDiffuseColor");
    ozcanInst.specularColorLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uPointLightingSpecularColor");
    ozcanInst.emissiveColorLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uPointLightingEmissiveColor");
    ozcanInst.shininessLocation = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uMaterialShininess");
    CHECK_GL_ERROR;
    ozcanInst.oldCodeTextureSelection = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uTextureRender"); /////
    ozcanInst.oldCodeLightingSelection = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uLightingRender"); /////
    CHECK_GL_ERROR;
    // uniforms for rendering depth texture
    ozcanInst.oldCodeVertexRenderSelector = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uMainRenderVertexShader");
    ozcanInst.oldCodeFragmentRenderSelector = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uMainRenderFragmentShader");
    
    ozcanInst.lightViewProjectionMatrixUniform = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uViewProjectionLightMatrix");
    ozcanInst.depthTextureSamplerUniform = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uShadowMap");
    
    // point light uniforms
    ozcanInst.pointLightingLocationUniform = glGetUniformLocation(ozcanInst.theOnlyProgramID, "uPointLightingLocation");
    CHECK_GL_ERROR;
    
//    OpenGLImplementation& imp = Globals::Instance().gl.implementation;
//    ozcanInst.textureSamplerLocation = imp.textureSamplerLocation;
//    ozcanInst.projectionMatrixLocation = imp.projectionMatrixLocation;
//    ozcanInst.modelMatrixLocation =Globals::Instance().gl.implementation.modelMatrixLocation;
//    ozcanInst.viewMatrixLocation =Globals::Instance().gl.implementation.viewMatrixLocation;
//    ozcanInst.ambientColorLocation =Globals::Instance().gl.implementation.ambientColorLocation;
//    ozcanInst.diffuseColorLocation =Globals::Instance().gl.implementation.diffuseColorLocation;
//    ozcanInst.specularColorLocation =Globals::Instance().gl.implementation.specularColorLocation;
//    ozcanInst.emissiveColorLocation =Globals::Instance().gl.implementation.emissiveColorLocation;
//    ozcanInst.shininessLocation =Globals::Instance().gl.implementation.shininessLocation;
//    ozcanInst.oldCodeTextureSelection = imp.oldCodeTextureSelection;
//    ozcanInst.oldCodeLightingSelection = imp.oldCodeLightingSelection;
//    ozcanInst.oldCodeVertexRenderSelector = imp.GetUniformLocation("uMainRenderVertexShader");
//    ozcanInst.oldCodeFragmentRenderSelector = imp.GetUniformLocation("uMainRenderFragmentShader");
//    ozcanInst.lightViewProjectionMatrixUniform = imp.GetUniformLocation("uViewProjectionLightMatrix");
//    ozcanInst.depthTextureSamplerUniform = imp.GetUniformLocation("uShadowMap");
//    ozcanInst.pointLightingLocationUniform = imp.GetUniformLocation("uPointLightingLocation");
}

bool OriginalInitRenderToTexture() {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &(originalInst.FramebufferName));
    glBindFramebuffer(GL_FRAMEBUFFER, originalInst.FramebufferName);
    
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &(originalInst.depthTexture));
    glBindTexture(GL_TEXTURE_2D, originalInst.depthTexture);
    CHECK_GL_ERROR;
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, originalInst.depthTexture, 0);
    
    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
    
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }
    return true;
}

bool OzcanInitRenderToTexture() {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &(ozcanInst.framebufferName));
    CHECK_GL_ERROR;
    glBindFramebuffer(GL_FRAMEBUFFER, ozcanInst.framebufferName); // start using our new frame buffer
    CHECK_GL_ERROR;
    
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &(ozcanInst.depthTexture));
    glBindTexture(GL_TEXTURE_2D, ozcanInst.depthTexture);
    CHECK_GL_ERROR;
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, textureSize, textureSize, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ozcanInst.depthTexture, 0);
    CHECK_GL_ERROR;
    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    CHECK_GL_ERROR;
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }
    return true;
}

void OriginalPreRenderDepthToTexture() {
//    // Render to our framebuffer
//    glBindFramebuffer(GL_FRAMEBUFFER, originalInst.FramebufferName);
//    glViewport(0,0,textureSize,textureSize); // Render on the whole framebuffer, complete from the lower left corner to the upper right
//    
//    // We don't use bias in the shader, but instead we draw back faces,
//    // which are already separated from the front faces by a small distance
//    // (if your geometry is made this way)
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
//    
//    // Clear the screen
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    
//    // Use our shader
//    glUseProgram(originalInst.depthProgramID);
//    
//    // Compute the MVP matrix from the light's point of view
//    originalInst.depthProjectionMatrix = CMatrix::CreateOrthographicProjection(-30,30,-30,30,-30,40); // (-10,10,-10,10,-10,20);
//    originalInst.depthViewMatrix = CMatrix::LookAt(lightInvDir, CVector(0,0,0), CVector(0,1,0));
//    // or, for spot light :
//    //glm::vec3 lightPos(5, 20, 20);
//    //glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
//    //glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));
//    
//    originalInst.depthModelMatrix = CMatrix();
//    originalInst.depthMVP = originalInst.depthProjectionMatrix * originalInst.depthViewMatrix * originalInst.depthModelMatrix;
//    
//    // Send our transformation to the currently bound shader,
//    // in the "MVP" uniform
//    glUniformMatrix4fv(originalInst.depthMatrixID, 1, GL_FALSE, &(originalInst.depthMVP[0]));
    
    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, originalInst.FramebufferName);
    glViewport(0,0,textureSize,textureSize); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    // We don't use bias in the shader, but instead we draw back faces,
    // which are already separated from the front faces by a small distance
    // (if your geometry is made this way)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(originalInst.depthProgramID);
    
    // Compute the MVP matrix from the light's point of view
    originalInst.depthProjectionMatrix = glm::ortho<float>(-30,30,-30,30,-30,40); // (-10,10,-10,10,-10,20);
    originalInst.depthViewMatrix = glm::lookAt(lightInvDirGLM, glm::vec3(0,0,0), glm::vec3(0,1,0));
    // or, for spot light :
    //glm::vec3 lightPos(5, 20, 20);
    //glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
    //glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));
    
    originalInst.depthModelMatrix = glm::mat4(1.0);
    originalInst.depthMVP = originalInst.depthProjectionMatrix * originalInst.depthViewMatrix * originalInst.depthModelMatrix;
    
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(originalInst.depthMatrixID, 1, GL_FALSE, &(originalInst.depthMVP[0][0]));
}

void OzcanPreRenderDepthToTexture() {
    glBindFramebuffer(GL_FRAMEBUFFER, ozcanInst.framebufferName);
    CHECK_GL_ERROR;
    glViewport(0,0,textureSize,textureSize); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    glUniform1i(ozcanInst.oldCodeVertexRenderSelector, 1);
    glUniform1i(ozcanInst.oldCodeFragmentRenderSelector, 1);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUniformMatrix4fv(ozcanInst.projectionMatrixLocation, 1, GL_FALSE, &(CMatrix::CreateOrthographicProjection(-30,30,-30,30,-30,40).elements[0]));
    glUniformMatrix4fv(ozcanInst.viewMatrixLocation, 1, GL_FALSE, &(CMatrix::LookAt(lightInvDir, CVector(0,0,0), CVector::unitY).elements[0]));
    glUniformMatrix4fv(ozcanInst.modelMatrixLocation, 1, GL_FALSE, &(CMatrix().elements[0]));
}

void OriginalPreRender() {
//    // Render to the screen
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0,0,windowWidth,windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
//    
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
//    
//    // Clear the screen
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    
//    // Use our shader
//    glUseProgram(originalInst.programID);
//    
//    // Compute the MVP matrix from keyboard and mouse input
//    CMatrix ProjectionMatrix = CMatrix::CreatePerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
//    
//    CVector position( -16, 8, 1 );
//    // Initial horizontal angle : toward -Z
//    float horizontalAngle = 1.6f;
//    // Initial vertical angle : none
//    float  verticalAngle = -0.40f;
//    CVector direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
//    
//    CMatrix ViewMatrix = CMatrix::LookAt(position, position+direction, CVector::unitY);
//    //ViewMatrix = glm::lookAt(glm::vec3(14,6,4), glm::vec3(0,1,0), glm::vec3(0,1,0));
//    CMatrix ModelMatrix;
//    CMatrix MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
//    
//    CMatrix biasMatrix(
//                         0.5, 0.0, 0.0, 0.0,
//                         0.0, 0.5, 0.0, 0.0,
//                         0.0, 0.0, 0.5, 0.0,
//                         0.5, 0.5, 0.5, 1.0
//                         );
//    
//    CMatrix depthBiasMVP = biasMatrix*originalInst.depthMVP;
//    
//    // Send our transformation to the currently bound shader,
//    // in the "MVP" uniform
//    glUniformMatrix4fv(originalInst.MatrixID, 1, GL_FALSE, &MVP[0]);
//    glUniformMatrix4fv(originalInst.ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0]);
//    glUniformMatrix4fv(originalInst.ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0]);
//    glUniformMatrix4fv(originalInst.DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0]);
//    
//    glUniform3f(originalInst.lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);
//    
//    // Bind our texture in Texture Unit 0
//    glActiveTexture(GL_TEXTURE0);
//    CHECK_GL_ERROR;
//    glBindTexture(GL_TEXTURE_2D, Texture);
//    CHECK_GL_ERROR;
//    // Set our "myTextureSampler" sampler to user Texture Unit 0
//    glUniform1i(originalInst.TextureID, 0);
//    
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, originalInst.depthTexture);
//    CHECK_GL_ERROR;
//    glUniform1i(originalInst.ShadowMapID, 1);
    
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,windowWidth,windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(originalInst.programID);
    
    // Compute the MVP matrix from keyboard and mouse input
    glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    
    glm::vec3 position( -16, 8, 1 );
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 1.6f;
    // Initial vertical angle : none
    float  verticalAngle = -0.40f;
    glm::vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
    
    glm::mat4 ViewMatrix = glm::lookAt(position, position+direction, glm::vec3(0, 1, 0));
    //ViewMatrix = glm::lookAt(glm::vec3(14,6,4), glm::vec3(0,1,0), glm::vec3(0,1,0));
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    
    glm::mat4 biasMatrix(
                         0.5, 0.0, 0.0, 0.0,
                         0.0, 0.5, 0.0, 0.0,
                         0.0, 0.0, 0.5, 0.0,
                         0.5, 0.5, 0.5, 1.0
                         );
    
    glm::mat4 depthBiasMVP = biasMatrix*originalInst.depthMVP;
    
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(originalInst.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(originalInst.ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(originalInst.ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniformMatrix4fv(originalInst.DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
    
    glUniform3f(originalInst.lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, Texture);
    CHECK_GL_ERROR;
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(originalInst.TextureID, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, originalInst.depthTexture);
    CHECK_GL_ERROR;
    glUniform1i(originalInst.ShadowMapID, 1);
}

void OzcanPreRender() {
    glUniformMatrix4fv(ozcanInst.projectionMatrixLocation, 1, GL_FALSE, &(CMatrix::CreatePerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 100.0f).elements[0]));
    
    CVector position( -16, 8, 1 );
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 1.6f;
    // Initial vertical angle : none
    float  verticalAngle = -0.40f;
    CVector direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
    glUniformMatrix4fv(ozcanInst.viewMatrixLocation, 1, GL_FALSE, &(CMatrix::LookAt(position, position+direction, CVector::unitY).elements[0]));
    glUniformMatrix4fv(ozcanInst.modelMatrixLocation, 1, GL_FALSE, &(CMatrix().elements[0]));
    
    glUniform1i(ozcanInst.oldCodeVertexRenderSelector, 0);
    glUniform1i(ozcanInst.oldCodeFragmentRenderSelector, 0);
    
    glViewport(0, 0, windowWidth, windowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER,0); // go back to using the default frame buffer
    CHECK_GL_ERROR;
    CMatrix biasMatrix(
                         0.5, 0.0, 0.0, 0.0,
                         0.0, 0.5, 0.0, 0.0,
                         0.0, 0.0, 0.5, 0.0,
                         0.5, 0.5, 0.5, 1.0
                         );
    
    CMatrix lightMVP = biasMatrix * CMatrix::CreateOrthographicProjection(-30,30,-30,30,-30,40) * CMatrix::LookAt(lightInvDir, CVector(0,0,0), CVector(0,1,0));
    
    glUniformMatrix4fv(ozcanInst.lightViewProjectionMatrixUniform, 1, GL_FALSE, &(lightMVP.elements[0]));
    
    glActiveTexture(GL_TEXTURE1);
    CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, ozcanInst.depthTexture); // bind texture required
    CHECK_GL_ERROR;
    glUniform1i(ozcanInst.depthTextureSamplerUniform, 1);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    CVector4 lightPos(lightInvDir.x * 1000.0f, lightInvDir.y * 1000.0f, lightInvDir.z * 1000.0f, 1.0f);
    lightPos = CMatrix::LookAt(position, position+direction, CVector::unitY) * CVector4(lightPos.v3.x, lightPos.v3.y, lightPos.v3.z, 1.0f);
    glUniform3f(ozcanInst.pointLightingLocationUniform, lightPos.v3.x, lightPos.v3.y, lightPos.v3.z);
    CHECK_GL_ERROR;
    glUniform3f(ozcanInst.ambientColorLocation, 0.1f, 0.1f, 0.1f);
    glUniform3f(ozcanInst.diffuseColorLocation, 0.8f, 0.8f, 0.8f);
    glUniform3f(ozcanInst.specularColorLocation, 1.0f, 1.0f, 1.0f);
    glUniform4f(ozcanInst.emissiveColorLocation, 0.8f, 0.8f, 0.8f, 1.0f);
    glUniform1f(ozcanInst.shininessLocation, 1.652);
    CHECK_GL_ERROR;
    glUniform1i(ozcanInst.oldCodeTextureSelection, 1);
    glUniform1i(ozcanInst.oldCodeLightingSelection, 1);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    CHECK_GL_ERROR;
    glBindTexture(GL_TEXTURE_2D, Texture);
    CHECK_GL_ERROR;
    // Set our "myTextureSampler" sampler to user Texture Unit 0
    glUniform1i(ozcanInst.textureSamplerLocation, 0);
}

void OriginalDelete() {
    glDeleteProgram(originalInst.programID);
    glDeleteProgram(originalInst.depthProgramID);
    
    glDeleteFramebuffers(1, &(originalInst.FramebufferName));
    glDeleteTextures(1, &(originalInst.depthTexture));
}

void OzcanDelete() {
    glDeleteProgram(ozcanInst.theOnlyProgramID);
    
    glDeleteFramebuffers(1, &(ozcanInst.framebufferName));
    glDeleteTextures(1, &(ozcanInst.depthTexture));
}
// ---------

bool original = true;

int InitTest() {
    // We would expect width and height to be 1024 and 768
    // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get the actual framebuffer size:
    glfwGetFramebufferSize(osxWindow, &windowWidth, &windowHeight);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    CHECK_GL_ERROR;
    
    const char* version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cout << "\n" << version;
    
    // Ensure we can capture the escape key being pressed below
    //glfwSetInputMode(osxWindow, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //glfwSetInputMode(osxWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    //glfwPollEvents();
    //glfwSetCursorPos(osxWindow, 1024/2, 768/2);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    if (original) {
        OriginalLoadShaders();
        OriginalGetUniformLocation();
    } else {
        OzcanLoadShaders();
        OzcanGetUniformLocation();
    }
    
    // Load the texture
    Texture = loadDDS("uvmap.DDS");
    CHECK_GL_ERROR;
    
    // Read our .obj file
    loadOBJ("room_thickwalls.obj", vertices, uvs, normals);
    
    indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
    
    // Load it into a VBO
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(CVector), &indexed_vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(CVector2), &indexed_uvs[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(CVector), &indexed_normals[0], GL_STATIC_DRAW);
    
    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    
    if (original) {
        if (!OriginalInitRenderToTexture()) {
            return -1;
        }
    } else {
        if (!OzcanInitRenderToTexture()) {
            return -1;
        }
    }

    return 0;
}

void RenderTest() {
    if (original) {
        OriginalPreRenderDepthToTexture();
    } else {
        OzcanPreRenderDepthToTexture();
    }

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,  // The attribute we want to configure
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    
    glDrawElements(GL_TRIANGLES,      // mode
                   (int)indices.size(),    // count
                   GL_UNSIGNED_SHORT, // type
                   (void*)0           // element array buffer offset
                   );
    
    glDisableVertexAttribArray(0);

    if (original) {
        OriginalPreRender();
    } else {
        OzcanPreRender();
    }

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
                          1,                                // attribute
                          2,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
                          2,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    
    // Draw the triangles !
    glDrawElements(
                   GL_TRIANGLES,      // mode
                   (int)indices.size(),    // count
                   GL_UNSIGNED_SHORT, // type
                   (void*)0           // element array buffer offset
                   );
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

}

void DeleteTest() {
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteTextures(1, &Texture);
    
    if (original) {
        OriginalDelete();
    } else {
        OzcanDelete();
    }
    
    glDeleteVertexArrays(1, &VertexArrayID);
}