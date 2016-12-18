// CKeyMap.cpp - keymap implementation class

#ifdef OSX

//--- System includes ---------------
#include </usr/local/Cellar/glfw3/3.1.2/include/GLFW/glfw3.h>
//----------------------------------

//--- Header files ---------
#include "CKeyMapOSX.h" // the header file for this class
#include "Game/ReplayManager.h"
//--------------------------

CKeyMap::CKeyMap() // constructor
{
    for (int counter = 0; counter < eKeyCode_Max; ++counter)
    {
        keyboard_state[counter] = false;
        previousKeyboard_state[counter] = false;
        changed_state_pressed[counter] = false;
        changed_state_released[counter] = false;
    }
}

CKeyMap::~CKeyMap() //destructor
{
}

bool CKeyMap::Init()
{
	return true; // success
}

void CKeyMap::Update()
{
    glfwPollEvents();
    
    for (int counter = 0; counter < eKeyCode_Max; ++counter)
    {
        int glfwKeyCode = MapToGLFW((eKeyCode)counter);
        if (GLFW_KEY_UNKNOWN != glfwKeyCode) {
            keyboard_state[counter] = (GLFW_PRESS == glfwGetKey(osxWindow, glfwKeyCode));
            
            if (keyboard_state[counter]) {
                keyboard_state[counter] = keyboard_state[counter];
            }
        }
        
        // for all keyboard Globals::Instance().keys, if they were not pressed in the previous cycle
        // and they are pressed now set the appropriate position in the
        // changed_state_pressed array to true
        if (!previousKeyboard_state[counter] && keyboard_state[counter]){
            changed_state_pressed[counter] = true;}
        else{
            changed_state_pressed[counter] = false;}
            
        // for all keyboard Globals::Instance().keys, if they were pressed in the previous cycle
        // and they are not pressed now set the appropriate position in the
        // changed_state_unpressed array to true
        if (previousKeyboard_state[counter] && !keyboard_state[counter]){
            changed_state_released[counter] = true;}
        else{
            changed_state_released[counter] = false;}
            
        // set the previousKeyboard_state array elements to the keyboard_state
        // array
        previousKeyboard_state[counter] = keyboard_state[counter];
    }// end for counter
}

bool CKeyMap::GetKeyPressed(eKeyCode key)
{
    // see if the input key sent as a parameter
    // was not pressed in the previous cycle and
    // is now pressed in this cycle
    return changed_state_pressed[key];
}

bool CKeyMap::GetKeyReleased(eKeyCode key)
{
    // see if the input key sent as a parameter
    // was pressed in the previous cycle and
    // is now not pressed in this cycle
    return changed_state_released[key];
}

bool CKeyMap::GetKeyStatus(eKeyCode key)
{
    // see if the key sent as a parameter
    // has been pressed this cycle
    return keyboard_state[key];
}

int CKeyMap::MapToGLFW(eKeyCode key)
{
    switch (key)
    {
        case eKeyCode_RETURN: return GLFW_KEY_ENTER;
        case eKeyCode_C: return GLFW_KEY_C;
        case eKeyCode_LSHIFT: return GLFW_KEY_LEFT_SHIFT;
        case eKeyCode_RSHIFT: return GLFW_KEY_RIGHT_SHIFT;
        case eKeyCode_P: return GLFW_KEY_P;
        case eKeyCode_J: return GLFW_KEY_J;
        case eKeyCode_L: return GLFW_KEY_L;
        case eKeyCode_I: return GLFW_KEY_I;
        case eKeyCode_K: return GLFW_KEY_K;
        case eKeyCode_R: return GLFW_KEY_R;
        case eKeyCode_T: return GLFW_KEY_T;
        case eKeyCode_F: return GLFW_KEY_F;
        case eKeyCode_A: return GLFW_KEY_A;
        case eKeyCode_D: return GLFW_KEY_D;
        case eKeyCode_E: return GLFW_KEY_E;
        case eKeyCode_Q: return GLFW_KEY_Q;
        case eKeyCode_W: return GLFW_KEY_W;
        case eKeyCode_S: return GLFW_KEY_S;
        case eKeyCode_N: return GLFW_KEY_N;
        case eKeyCode_TAB: return GLFW_KEY_TAB;
        case eKeyCode_UP: return GLFW_KEY_UP;
        case eKeyCode_DOWN: return GLFW_KEY_DOWN;
        case eKeyCode_LEFT: return GLFW_KEY_LEFT;
        case eKeyCode_RIGHT: return GLFW_KEY_RIGHT;
        case eKeyCode_SPACE: return GLFW_KEY_SPACE;
        case eKeyCode_M: return GLFW_KEY_M;
        case eKeyCode_NUMPAD0: return GLFW_KEY_KP_0;
        case eKeyCode_NUMPAD1: return GLFW_KEY_KP_1;
        case eKeyCode_NUMPAD2: return GLFW_KEY_KP_2;
        case eKeyCode_NUMPAD3: return GLFW_KEY_KP_3;
        case eKeyCode_NUMPAD4: return GLFW_KEY_KP_4;
        case eKeyCode_NUMPAD5: return GLFW_KEY_KP_5;
        case eKeyCode_1: return GLFW_KEY_1;
        case eKeyCode_2: return GLFW_KEY_2;
        default: break;
    }
    return GLFW_KEY_UNKNOWN;
}

#endif