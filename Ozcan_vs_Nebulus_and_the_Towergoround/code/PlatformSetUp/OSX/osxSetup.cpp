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

GLFWwindow* osxWindow = nullptr;
bool quitApp = false;

void SetCurrentWorkingDirectory(int argc, char *argv[])
{
    bool currenlyInDebugger = argc > 1 && 0 == strcmp(argv[1], "xcodeDebugger");
    // we only want to set the cwd when running outside the debugger. inside debugger, xcode sets
    // our cwd for us
    if (!currenlyInDebugger)
    {
        std::cout << "\n\n" << "exe path from command: " << argv[0] << "\n\n";
        
        const int FilePathLength = 2048;
        char executablePath[FilePathLength];
        
        strcpy(executablePath, argv[0]); // get the exe path
        
        // the path passed in includes the exe name, so this section trims that off
        char * lastForwardSlash = strrchr(executablePath,'/');
        if (lastForwardSlash) {
            *(lastForwardSlash + 1) = '\0'; // remove the exe name from end of path
        }
        
        std::cout << "\n\n" << "requested new cwd: " << executablePath << "\n\n";
        chdir(executablePath); //change cwd
        
        // this section prints out the new cwd which we have just set
        char currentWorkingDirectory[FilePathLength];
        char* cwd = getcwd(currentWorkingDirectory, FilePathLength);
        if (cwd) {
            std::cout << "\n\n" << "current working directory: " << cwd << "\n\n";
        }
        else {
            std::cout << "\n\n" << "current working directory: NONE" << "\n\n";
        }
    }
}

int main(int argc, char *argv[])
{
    SetCurrentWorkingDirectory(argc, argv);
    
    if (!glfwInit())
    {
        return -1;
    }
    //glEnable(GL_DEPTH_TEST);
    
    int width = 1024;
    int height = 768;
    osxWindow = glfwCreateWindow( width, height, "Ozcan vs Nebulus & the Towergoround", NULL, NULL);
    if (!osxWindow)
    {
        glfwTerminate(); //terminating glfw window
        return -2;
    }
    
    glfwMakeContextCurrent(osxWindow);
    
    if (!Globals::Instance().gl.InitGL())
    {
        Text::DeleteMesh();
        Globals::Instance().gl.DestroyGL();
        glfwTerminate(); //terminating glfw window
        return 0;// quit if window was not created
    }
    
    int modelMatrixLocation = Globals::Instance().gl.GetUniformLocation("uModelMatrix");
    Globals::Instance().modelMatrixStack.SetMatrixLocation(modelMatrixLocation);
    int viewMatrixLocation = Globals::Instance().gl.GetUniformLocation("uViewMatrix");
    Globals::Instance().viewMatrixStack.SetMatrixLocation(viewMatrixLocation);
    
    if (Globals::Instance().gl.IsUsingSubRoutines())
    {
        Globals::Instance().gl.SetSubroutineUniformIndex(Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_FRAGMENT_SHADER),
                                                     Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_FRAGMENT_SHADER), GL_FRAGMENT_SHADER);
        Globals::Instance().gl.SetSubroutineUniformIndex(Globals::Instance().gl.GetSubroutineUniformIndex("mainRender", GL_VERTEX_SHADER),
                                                     Globals::Instance().gl.GetSubroutineIndex("RenderScene", GL_VERTEX_SHADER), GL_VERTEX_SHADER);
    }
    else
    {
        int mainRenderVertexShaderLocation = Globals::Instance().gl.GetUniformLocation("uMainRenderVertexShader");
        Globals::Instance().gl.SetUniformBool(mainRenderVertexShaderLocation, false);
        
        int mainRenderFragmentShaderLocation = Globals::Instance().gl.GetUniformLocation("uMainRenderFragmentShader");
        Globals::Instance().gl.SetUniformBool(mainRenderFragmentShaderLocation, false);
    }
    
    int viewProjectionLightMatrixLocation = Globals::Instance().gl.GetUniformLocation("uViewProjectionLightMatrix");
    Globals::Instance().gl.SetUniformMatrix(viewProjectionLightMatrixLocation, CMatrix());

    int frameBufferWidth = 0;
    int frameBufferHeight = 0;
    glfwGetFramebufferSize(osxWindow, &frameBufferWidth, &frameBufferHeight);
    
    Globals::Instance().gl.ReSizeGLScene(frameBufferWidth, frameBufferHeight);
    CMenu::SetPerspectiveProjectionMatrix();
    
    Game game;
    if(!game.Init())
    {
        Text::DeleteMesh();
        Globals::Instance().gl.DestroyGL(); // must be donw before killing the game window
        glfwTerminate(); //terminating glfw window
        return (0); // failure
    }
    
    Globals::Instance().sound.PlaySound( SOUNDS_MAINMUSIC, true, false ); //start repeatedly playing main music

    while (!glfwWindowShouldClose(osxWindow))
    {
        if (!game.Update() || quitApp) {
            break;
        }
        
//#if _DEBUG
//        Globals::Instance().debug.printDebug();
//#endif
        glfwSwapBuffers(osxWindow);
    }

#ifdef USE_SHADERS
    CHECK_GL_ERROR;
#endif
    
    // Shutdown
    game.DeleteGameObjects(); // delete all game specific objects
    Text::DeleteMesh();
    Globals::Instance().gl.DestroyGL();
    glfwTerminate(); //terminating glfw window
    return 0;
}