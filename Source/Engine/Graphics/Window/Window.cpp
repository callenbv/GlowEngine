/*
/
// filename: Window.cpp
// author: Callen Betts
// brief: implements Window.h
/
*/

#include "stdafx.h"
#include "Window.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"

// construct the window
Graphics::Window::Window()
  : 
    aspectRatio(1),
    engine(nullptr),
    input(nullptr),
    windowHandle(nullptr),
    hInstance(nullptr)
{
    SetProcessDPIAware();
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    windowWidth = static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 1.25f);
    windowHeight = static_cast<int>(GetSystemMetrics(SM_CYSCREEN) / 1.25f);
    windowClassName = L"Otherglow Window";
    windowName = L"Otherglow";
}

// setup the window and register it in windows
bool Graphics::Window::setup()
{
  // register the window class
  hInstance = GetModuleHandle(nullptr);

  WNDCLASSEX wc = {};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = windowProcStatic;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.lpszClassName = windowClassName.c_str();

  RegisterClassEx(&wc);

  // Create window handle
  windowHandle = CreateWindowEx(
    0, // window style
    windowClassName.c_str(), // class name
    windowName.c_str(), // window title
    WS_OVERLAPPEDWINDOW, // window style
    CW_USEDEFAULT, // initial horizontal position of the window
    CW_USEDEFAULT, // initial vertical position of the window
    windowWidth, // width
    windowHeight, // height
    nullptr, // window parent (none)
    nullptr, // window child (none)
    hInstance, // window instance handle
    this // Set the GWLP_USERDATA to the GlowEngine instance
  );

  if (windowHandle == nullptr)
  {
    return false;
  }

  // position the window to be in the centre of the screen
  RECT rect = { 0, 0, static_cast<LONG>(windowWidth), static_cast<LONG>(windowHeight) };
  AdjustWindowRect(&rect, GetWindowLong(windowHandle, GWL_STYLE), FALSE);
  SetWindowPos(windowHandle, nullptr, 64, 164, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

  aspectRatio = (float)windowWidth / (float)windowHeight;

  return true;
}

// show and update the window
// update the window with it's system pointers 
void Graphics::Window::updateWindow()
{
  // window refresh
  UpdateWindow(windowHandle);
  ShowWindow(windowHandle, true);
  // system initialization
  engine = EngineInstance::getEngine();
  input = engine->getInputSystem();
}

// helpful static functions for getting width and height of the window
int Graphics::Window::GetHeight()
{
  return EngineInstance::getEngine()->getRenderer()->getWindow()->getHeight();
}

int Graphics::Window::GetWidth()
{
  return EngineInstance::getEngine()->getRenderer()->getWindow()->getWidth();
}

// windows message callback
LRESULT Graphics::Window::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    return true;

  // handle input system key states
  if (input)
    input->updateKeyStates();

  switch (message)
  {
    // when a key is first triggered, set the keystate to active
  case WM_KEYDOWN:
    input->onKeyTriggered(static_cast<int>(wParam));
    break;

    // when a key is released, you should reset the keystate and call "released"
  case WM_KEYUP:
    input->onKeyRelease(static_cast<int>(wParam));
    break;

    // Mouse wheel scrolled
  case WM_MOUSEWHEEL:
  {
    input->onMouseScroll(static_cast<int>(wParam));
    break;
  }

    // handle mouse clicking being held down
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN:
  {
    WPARAM button = 0;
    if (message == WM_LBUTTONDOWN)
      button = MK_LBUTTON;
    else if (message == WM_RBUTTONDOWN)
      button = MK_RBUTTON;
    else if (message == WM_MBUTTONDOWN)
      button = MK_MBUTTON;

    input->onKeyTriggered(static_cast<int>(button));
    break;
  }

    // handle mouse clicking being released
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
  {
    WPARAM button = 0;
    if (message == WM_LBUTTONUP)
      button = MK_LBUTTON;
    else if (message == WM_RBUTTONUP)
      button = MK_RBUTTON;
    else if (message == WM_MBUTTONUP)
      button = MK_MBUTTON;

    input->onKeyRelease(static_cast<int>(button));
    break;
  }

    // destroy the window and stop the engine
  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// windows callback function
LRESULT Graphics::Window::windowProcStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return EngineInstance::getEngine()->getWindow()->windowProc(hWnd, message, wParam, lParam);
}

void Graphics::Window::setFullscreen(bool val)
{
  if (val)
  {
    SetWindowLongPtr(windowHandle, GWL_STYLE, WS_BORDER | WS_VISIBLE);
    ShowWindow(windowHandle, SW_MAXIMIZE);
    Logger::write("Toggled fullscreen on");
  }
  else
  {
    SetWindowLongPtr(windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
    SetWindowPos(windowHandle, HWND_TOP, 0, 0, windowWidth, windowHeight, SWP_FRAMECHANGED);
    ShowWindow(windowHandle, SW_NORMAL);
    Logger::write("Toggled fullscreen off");
  }
}

// update messages, return false if we quit the window
bool Graphics::Window::updateMessages()
{
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (msg.message == WM_QUIT) 
    {
      return false;
    }
    return true;
  }
  return true;
}

int Graphics::Window::getWidth()
{
  return windowWidth;
}

int Graphics::Window::getHeight()
{
  return windowHeight;
}

// return the current message parameter (quit, create, etc)
int Graphics::Window::getMessageParam()
{
  return static_cast<int>(msg.wParam);
}
