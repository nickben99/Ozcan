//
//  osxSetup.cpp
//  Ozcan vs Nebulus and the Towergoround
//
//  Created by Benjamin Nickson on 7/9/16.
//  Copyright © 2016 Benjamin Nickson. All rights reserved.
//

#include </usr/local/Cellar/glew/1.13.0/include/GL/glew.h>
#include </usr/local/Cellar/glfw3/3.1.2/include/GLFW/glfw3.h>
#include "Audio/SoundsEnum.h"

void display()
{
    glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT);
    
    glBegin(GL_TRIANGLES); //Begin triangle coordinates
    
    glColor3f(0.0f, 1.0f, 0.0f);
    //Triangle
    glVertex2f(0.0f, 0.5f);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    
    glEnd(); //End triangle coordinates
    
}

int main()
{
    if (!glfwInit())
    {
        return -1;
    }
    //glEnable(GL_DEPTH_TEST);
    
    GLFWwindow* window = glfwCreateWindow( 640, 480, "Ozcan vs Nebulus and the Towergoround", NULL, NULL);
    if (!window)
    {
        glfwTerminate(); //terminating glfw window
        return -2;
    }
    
    glfwMakeContextCurrent(window);
    
    //glfwSetKeyCallback(handleKeypress); //callback function to handle keypress
    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate(); //terminating glfw window
    return 0;
}