/*
/
// filename: Window.h
// author: Callen Betts
// brief: defines Window class
/
*/

#pragma once

namespace Graphics
{

  class Window
  {

  public:

    Window();

    // setup the window
    bool setup();

    // update
    void updateWindow();

    static int GetHeight();
    static int GetWidth();

    // windows callbacks
    LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK windowProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    // set fullscreen
    void setFullscreen(bool val);

    // message updates
    bool updateMessages();

    HWND getHandle() { return windowHandle; }
    int getWidth();
    int getHeight();
    float getAspectRatio() { return aspectRatio; }

    const std::wstring getName() { return windowName; }
    void setName(std::wstring name) { windowName = name; }

    // get the message parameter from input
    int getMessageParam();

  private:

    // window properties
    std::wstring windowName;
    std::wstring windowClassName;
    HWND windowHandle;
    HINSTANCE hInstance;

    int windowWidth;
    int windowHeight;

    // windows messages
    MSG msg = {};

    float aspectRatio;

    // pointers
    Input::InputSystem* input;
    Engine::GlowEngine* engine;
  };

}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
