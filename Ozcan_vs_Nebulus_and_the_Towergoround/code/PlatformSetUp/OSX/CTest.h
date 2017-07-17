#ifndef _CTest_h_
#define _CTest_h_

struct GLFWwindow;
extern GLFWwindow* osxWindow;

int InitTest(bool load);
void SetOrtho();
void DrawOrtho();
void RenderTest();
void DeleteTest();

#endif // ifndef _CTest_h_
