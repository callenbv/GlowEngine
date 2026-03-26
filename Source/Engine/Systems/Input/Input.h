/*
/
// filename: Input.h
// author: Callen Betts
// brief: defines input system for handling basic input with mouse and keyboard
/
*/

#pragma once
#include "Game/System/System.h"

namespace Input
{

  class InputSystem : public Systems::System
  {

  public:

    InputSystem(std::string systemName);

    // static methods for getting input
    static bool KeyPressed(int key);
    static bool KeyReleased(int key);
    static bool KeyDown(int key);
    static bool MouseScrollUp();
    static bool MouseScrollDown();

    // clear the input
    void Clear();
    void Flush();

    // general game loop update
    void update();
    // update key states
    void updateKeyStates();
    // update hotkeys
    void updateHotkeys();
    // get the mouse's position
    POINT GetMousePosition();
    // disable pivot mode
    void ClearPivot();
    // update controller
    void UpdateController();
    
    // center the mouse
    void CenterMouse();
    // set the mouse pivot position
    void SetMousePivot(bool val);

    // get the mouse delta
    Vector3D getMouseDelta() { return { (float)mouseDelta.x, (float)mouseDelta.y,0 }; }

    // called when windows triggered a key
    void onKeyTriggered(int keycode);

    // called when windows released a key
    void onKeyRelease(int keycode);

    // called when our mouse was scrolled
    void onMouseScroll(int param);

    // clicking
    void onMouseClick(int param);

    // if a key is being held down
    bool keyDown(int key);

    // if a key was triggered
    bool keyTriggered(int key);

    // gets our scroll wheel delta (1 up, -1, down)
    int getMouseScrollDelta();

    // if a key was released
    bool keyReleased(int key);

    // if we are focused on this window
    bool isFocused() { return focused; }
    // set focused
    void setFocus(bool focus);

  private:

    // mouse data
    int scrollDelta;
    POINT mouseDelta;
    POINT currentMousePosition;
    POINT previousMousePosition;
    POINT pivotPoint;

    bool focused;
    bool pivot; // allows us to lock our mouse on a position, letting us 'pivot' if need be
    bool showCursor;

    HWND windowHandle;

    // for handling previous mouse states
    RECT clientRect;
    POINT center;

    // keystates
    std::unordered_map<int, bool> keystates;
    std::unordered_map<int, bool> previousKeystates;

  };

}