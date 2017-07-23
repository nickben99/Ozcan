//
//  osxSetup.cpp
//  Ozcan vs Nebulus and the Towergoround
//
//  Created by Benjamin Nickson on 7/9/16.
//  Copyright © 2016 Benjamin Nickson. All rights reserved.
//

#include <iostream>

#include </usr/local/Cellar/glew/1.13.0/include/GL/glew.h>
#include </usr/local/Cellar/glfw3/3.1.2/include/GLFW/glfw3.h>
#include "Audio/SoundsEnum.h"
#include <Game/Globals.h>
#include <Game/CMenu.h>
#include <Game/Game.h>
#include <FileReading/TEXTURE.H>

GLFWwindow* osxWindow = nullptr;
bool quitApp = false;

extern GLuint TextureLoad(const char *filename, bool alpha, GLenum minfilter, GLenum magfilter, GLenum wrap);

void SetCurrentWorkingDirectory(int argc, char *argv[])
{
    bool currenlyInDebugger = argc > 1 && 0 == strcmp(argv[1], "xcodeDebugger");
    // we only want to set the cwd when running outside the debugger. inside debugger, xcode sets
    // our cwd for us
    if (!currenlyInDebugger)
    {        
        const int FilePathLength = 2048;
        char executablePath[FilePathLength];
        
        strcpy(executablePath, argv[0]); // get the exe path
        
        // the path passed in includes the exe name, so this section trims that off
        char * lastForwardSlash = strrchr(executablePath,'/');
        if (lastForwardSlash) {
            *(lastForwardSlash + 1) = '\0'; // remove the exe name from end of path
        }
        
        chdir(executablePath); //change cwd
    }
}

void DrawTex()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear screen and depth buffer
    Globals::Instance().viewMatrixStack.LoadIdentity(); // reset the current modelview matrix
    Globals::Instance().modelMatrixStack.LoadIdentity(); // reset the current modelview matrix
    
    // set the screen to orthographic view
    CMenu::setToOrthograthicView();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
    Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)
    
    MeshBuffer backgroundMesh;
    
    // verts
    CVector	centre(0.75f*Globals::Instance().windowWidth, 0.75f*Globals::Instance().windowHeight, 0.0f);
    CVector dimensions(Globals::Instance().windowWidth*0.5f, Globals::Instance().windowHeight*0.5f, 0.0f);
    GLfloat vertexPositionArray[] = {
        ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f
    };
    backgroundMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
    
    float vertexTexCoordsArray[] =
    {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };
    
    backgroundMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
    
    unsigned short vertexIndecisArray[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    backgroundMesh.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
    //backgroundMesh.SetColor(CVector4(1.0f, 0.0f, 0.0f, 1.0f));
    backgroundMesh.SetTexture(4);
    
    backgroundMesh.Draw();
    
    // set the screen back to perspective view
    CMenu::setToPerspectiveView();
}

void DrawTex2()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear screen and depth buffer
    Globals::Instance().viewMatrixStack.LoadIdentity(); // reset the current modelview matrix
    Globals::Instance().modelMatrixStack.LoadIdentity(); // reset the current modelview matrix
    
    // set the screen to orthographic view
    CMenu::setToOrthograthicView();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
    Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)
    
    MeshBuffer backgroundMesh;
    
    // verts
    CVector	centre(0.25f*Globals::Instance().windowWidth, 0.25f*Globals::Instance().windowHeight, 0.0f);
    CVector dimensions(Globals::Instance().windowWidth*0.5f, Globals::Instance().windowHeight*0.5f, 0.0f);
    GLfloat vertexPositionArray[] = {
        ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f
    };
    backgroundMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
    
    float vertexTexCoordsArray[] =
    {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };
    
    backgroundMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
    
    unsigned short vertexIndecisArray[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    backgroundMesh.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
    //backgroundMesh.SetColor(CVector4(1.0f, 0.0f, 0.0f, 1.0f));
    backgroundMesh.SetTexture(5);
    
    backgroundMesh.Draw();
    
    // set the screen back to perspective view
    CMenu::setToPerspectiveView();
}

void DrawTex3()
{
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear screen and depth buffer
    Globals::Instance().viewMatrixStack.LoadIdentity(); // reset the current modelview matrix
    Globals::Instance().modelMatrixStack.LoadIdentity(); // reset the current modelview matrix
    
    // set the screen to orthographic view
    CMenu::setToOrthograthicView();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // blend type (for transparency)
    Globals::Instance().gl.GLEnable(GL_BLEND); // enable blending (for transparency)
    
    MeshBuffer backgroundMesh;
    
    // verts
    CVector	centre(0.25f*Globals::Instance().windowWidth, 0.75f*Globals::Instance().windowHeight, 0.0f);
    CVector dimensions(Globals::Instance().windowWidth*0.5f, Globals::Instance().windowHeight*0.5f, 0.0f);
    GLfloat vertexPositionArray[] = {
        ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x - dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y + dimensions.y*0.5f), 0.0f,
        ToShaderUIX(centre.x + dimensions.x*0.5f), ToShaderUIY(centre.y - dimensions.y*0.5f), 0.0f
    };
    backgroundMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
    
    float vertexTexCoordsArray[] =
    {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };
    
    backgroundMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));
    
    unsigned short vertexIndecisArray[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    backgroundMesh.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
    //backgroundMesh.SetColor(CVector4(1.0f, 0.0f, 0.0f, 1.0f));
    backgroundMesh.SetTexture(6);
    
    backgroundMesh.Draw();
    
    // set the screen back to perspective view
    CMenu::setToPerspectiveView();
}

int main(int argc, char *argv[])
{
    SetCurrentWorkingDirectory(argc, argv);
    
    if (!glfwInit())
    {
        return -1;
    }
    //glEnable(GL_DEPTH_TEST);
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    int width = 1024;
    int height = 768;
    osxWindow = glfwCreateWindow( width, height, "Ozcan vs Nebulus & the Towergoround", NULL, NULL);
    if (!osxWindow)
    {
        glfwTerminate(); //terminating glfw window
        return -2;
    }
    
    glfwMakeContextCurrent(osxWindow);
    
    bool runDemo = false;
    bool loadShadersInCTest = false;

    if (!runDemo || !loadShadersInCTest) {
        if (!Globals::Instance().gl.InitGL())
        {
            Text::DeleteMesh();
            Globals::Instance().gl.DestroyGL();
            glfwTerminate(); //terminating glfw window
            return 0;// quit if window was not created
        }
    }
    
    //if (runDemo) {
//        if (0 != InitTest(loadShadersInCTest)) {
//            return 0;
//        }
    //}

    Game* game = nullptr;
    MeshBuffer backgroundMesh;

        // load texture
        char theBackground[400];
        SPRINTF(theBackground, "%simages/back2.bmp", GetDirectoryPath());
        
        int background = // bind the background texture
        TextureLoad(theBackground, GL_FALSE, GL_LINEAR, GL_LINEAR, GL_REPEAT);
        
        if (background <= 0){ // if their were problems loading the background
            return (0);} // unsuccesful load
        
        // verts
        float mult = 0.25f;
        GLfloat vertexPositionArray[] = {
            -1024.0f*mult, -687.0f*mult, 0.0f,
            -1024.0f*mult, 687.0f*mult, 0.0f,
            1024.0f*mult, 687.0f*mult, 0.0f,
            1024.0f*mult, -687.0f*mult, 0.0f
        };
        
        backgroundMesh.CreateVertexArray(vertexPositionArray, sizeof(vertexPositionArray) / sizeof(float));
        
        float vertexTexCoordsArray[] =
        {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };
        backgroundMesh.CreateTexCoordArray(vertexTexCoordsArray, sizeof(vertexTexCoordsArray) / sizeof(float));

        float vertexNormalArray[] =
        {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f
        };
        backgroundMesh.CreateNormalArray(vertexNormalArray, sizeof(vertexNormalArray) / sizeof(float));
        
        
        unsigned short vertexIndecisArray[] =
        {
            0, 1, 2,
            0, 2, 3
        };
        backgroundMesh.CreateIndexArray(vertexIndecisArray, sizeof(vertexIndecisArray) / sizeof(unsigned short));
        backgroundMesh.SetTexture(background);
    
    if (!runDemo) {
        int modelMatrixLocation = Globals::Instance().gl.GetUniformLocation(Globals::Instance().gl.IsUsingSubRoutines() ? "uModelMatrix" : "M");
        Globals::Instance().modelMatrixStack.SetMatrixLocation(modelMatrixLocation);
        Globals::Instance().modelMatrixStack.LoadIdentity();
        int viewMatrixLocation = Globals::Instance().gl.GetUniformLocation(Globals::Instance().gl.IsUsingSubRoutines() ? "uViewMatrix" : "V");
        Globals::Instance().viewMatrixStack.SetMatrixLocation(viewMatrixLocation);
        Globals::Instance().viewMatrixStack.LoadIdentity();
    
        if (Globals::Instance().gl.IsUsingSubRoutines())
        {
        Globals::Instance().gl.SetSubroutineUniformIndex(Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_FRAGMENT_SHADER),
                                                     Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_FRAGMENT_SHADER), GL_FRAGMENT_SHADER);
        Globals::Instance().gl.SetSubroutineUniformIndex(Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_VERTEX_SHADER),
                                                     Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_VERTEX_SHADER), GL_VERTEX_SHADER);
            
            int viewProjectionLightMatrixLocation = Globals::Instance().gl.GetUniformLocation("uViewProjectionLightMatrix");
            Globals::Instance().gl.SetUniformMatrix(viewProjectionLightMatrixLocation, CMatrix());
        }
        else
        {
//            int mainRenderVertexShaderLocation = Globals::Instance().gl.GetUniformLocation("uMainRenderVertexShader");
//            Globals::Instance().gl.SetUniformBool(mainRenderVertexShaderLocation, false);
//        
//            int mainRenderFragmentShaderLocation = Globals::Instance().gl.GetUniformLocation("uMainRenderFragmentShader");
//            Globals::Instance().gl.SetUniformBool(mainRenderFragmentShaderLocation, false);
            
            int viewProjectionLightMatrixLocation = Globals::Instance().gl.GetUniformLocation("DepthBiasMVP");
            Globals::Instance().gl.SetUniformMatrix(viewProjectionLightMatrixLocation, CMatrix());
            
            int lightInvDirection = Globals::Instance().gl.GetUniformLocation("LightInvDirection_worldspace");
            Globals::Instance().gl.SetUniformVector3(lightInvDirection, CVector::unitY);
            
            int shadowMap = Globals::Instance().gl.GetUniformLocation("shadowMap");
            Globals::Instance().gl.SetUniformInt(shadowMap, 1);
        }

        int frameBufferWidth = 0;
        int frameBufferHeight = 0;
        glfwGetFramebufferSize(osxWindow, &frameBufferWidth, &frameBufferHeight);

        Globals::Instance().gl.ReSizeGLScene(frameBufferWidth, frameBufferHeight);
        CMenu::SetPerspectiveProjectionMatrix();
    
        game = new Game();
        if(!game->Init())
        {
            Text::DeleteMesh();
            Globals::Instance().gl.DestroyGL(); // must be donw before killing the game window
            Globals::Instance().sound.Shutdown();
            glfwTerminate(); //terminating glfw window
            return (0); // failure
        }
    
        Globals::Instance().sound.PlaySound( SOUNDS_MAINMUSIC, true, false ); //start repeatedly playing main music
    }
    
    while (!glfwWindowShouldClose(osxWindow))
    {
        if (!runDemo) {
            if (!game->Update() || quitApp) {
                break;
            }
        }
        
//#if _DEBUG
        //Globals::Instance().debug.printDebug();
//#endif
        if (runDemo) {
            Globals::Instance().keys.Update();
            //RenderTest();
            
            // render mesh buffer
            //SetOrtho();
            backgroundMesh.Draw();
        }
        
        glfwSwapBuffers(osxWindow);
    }

#ifdef USE_SHADERS
    CHECK_GL_ERROR;
#endif
    
    //DeleteTest();
    
    // Shutdown
//    game.DeleteGameObjects(); // delete all game specific objects
//    Text::DeleteMesh();
//    Globals::Instance().gl.DestroyGL();
//    Globals::Instance().sound.Shutdown();
    glfwTerminate(); //terminating glfw window
    
    if (!runDemo) {
        delete game;
    }
    
    return 0;
}