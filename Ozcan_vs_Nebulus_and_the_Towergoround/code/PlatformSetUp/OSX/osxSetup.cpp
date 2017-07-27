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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
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
    
    if (!Globals::Instance().gl.InitGL())
    {
        Text::DeleteMesh();
        Globals::Instance().gl.DestroyGL();
        glfwTerminate(); //terminating glfw window
        return 0;// quit if window was not created
    }
    
    int modelMatrixLocation = Globals::Instance().gl.GetUniformLocation("uModelMatrix");
    Globals::Instance().modelMatrixStack.SetMatrixLocation(modelMatrixLocation);
    Globals::Instance().modelMatrixStack.LoadIdentity();
    int viewMatrixLocation = Globals::Instance().gl.GetUniformLocation("uViewMatrix");
    Globals::Instance().viewMatrixStack.SetMatrixLocation(viewMatrixLocation);
    Globals::Instance().viewMatrixStack.LoadIdentity();

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
        Globals::Instance().sound.Shutdown();
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
    Globals::Instance().sound.Shutdown();
    glfwTerminate(); //terminating glfw window
    return 0;
}