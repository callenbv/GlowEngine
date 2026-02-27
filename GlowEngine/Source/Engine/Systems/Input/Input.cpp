/*
/
// filename: Input.cpp
// author: Callen Betts
// brief: implements the input system
/
*/

#include "stdafx.h"
#include "Input.h"
#include "Engine/GlowEngine.h"
#include "Engine/EngineInstance.h"
#include "Engine/Graphics/Renderer.h"
#include "Game/Scene/SceneSystem.h"

Input::InputSystem::InputSystem(std::string systemName) 
  : System(systemName)
{
  windowHandle = EngineInstance::getEngine()->getWindowHandle();
  previousMousePosition = { 0 };
  currentMousePosition = { 0 };
  mouseDelta = { 0 };
  scrollDelta = 0;
  focused = true;
  pivot = false;
  showCursor = true;
  isSimulation = true;
  center = { 0, 0 };
  clientRect = { 0,0,0,0 };
  pivotPoint = { 0,0 };
}

bool Input::InputSystem::KeyDown(int key)
{
  return EngineInstance::getEngine()->getInputSystem()->keyDown(key);
}

bool Input::InputSystem::MouseScrollUp()
{
  return EngineInstance::getEngine()->getInputSystem()->getMouseScrollDelta() > 0;
}

bool Input::InputSystem::MouseScrollDown()
{
  return EngineInstance::getEngine()->getInputSystem()->getMouseScrollDelta() < 0;
}

void Input::InputSystem::Clear()
{
  // reset our keystates
  previousKeystates = keystates;
  scrollDelta = 0;
}

// Flushes all input
void Input::InputSystem::Flush()
{
  previousKeystates.clear();
  keystates.clear();
  scrollDelta = 0;
}

bool Input::InputSystem::KeyPressed(int key)
{
  return EngineInstance::getEngine()->getInputSystem()->keyTriggered(key);
}

bool Input::InputSystem::KeyReleased(int key)
{
  return EngineInstance::getEngine()->getInputSystem()->keyReleased(key);
}

void Input::InputSystem::UpdateController()
{
  // if we are pivoting, lock our mouse position; also make sure to recalculate delta x and y to be relative to the pivot point
  if (pivot)
  {
    showCursor = false;

    // calculate mouse position and delta 
    if (GetCursorPos(&currentMousePosition))
    {
      mouseDelta.x = currentMousePosition.x - pivotPoint.x;
      mouseDelta.y = currentMousePosition.y - pivotPoint.y;

      previousMousePosition = currentMousePosition;
      SetCursorPos(pivotPoint.x, pivotPoint.y);
    }
    else
    {
      mouseDelta.x = 0;
      mouseDelta.y = 0;
    }
  }
  else if (engine->isPlaying() && !engine->IsPaused())
  {
    showCursor = false;

    // game mouse controller
    if (GetCursorPos(&currentMousePosition))
    {
      // Calculate mouse delta
      mouseDelta.x = currentMousePosition.x - previousMousePosition.x;
      mouseDelta.y = currentMousePosition.y - previousMousePosition.y;
    }
    // focus the mouse in the center of the screen
    GetClientRect(windowHandle, &clientRect);
    center = { (clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2 };
    ClientToScreen(windowHandle, &center);
    SetCursorPos(center.x, center.y);

    // update previous mouse position to the center
    previousMousePosition.x = center.x;
    previousMousePosition.y = center.y;

  }
  else
  {
    showCursor = true;

    // default controller
    GetCursorPos(&currentMousePosition);
    previousMousePosition = currentMousePosition;
  }

}

// update the input state
void Input::InputSystem::update()
{
  UpdateController();
}

void Input::InputSystem::updateKeyStates()
{
  // update any of our hotkeys
  updateHotkeys();
}

void Input::InputSystem::updateHotkeys()
{
  if (showCursor)
    while (ShowCursor(TRUE) <= 0);
  else
    while (ShowCursor(FALSE) > 0);

  // toggle fullscreen
  if (keyReleased(VK_TAB))
  {  
    // Flush input so no spam
    Flush();
    engine->getRenderer()->toggleFullscreen();
  }

  // quick save
  if (keyDown(VK_CONTROL))
  {
    if (keyTriggered('S'))
    {
      engine->getSceneSystem()->getCurrentScene()->SaveScene();
    }
  }

  // terminate engine on escape
  if (keyTriggered(VK_ESCAPE))
  {
    if (!engine->isPlaying())
    {
      engine->stop();
    }
    else
    {
      engine->SetPaused(true);
    }
  }

  // refocus
  if (keyTriggered(VK_RETURN))
  {
    if (engine->isPlaying())
    {
      engine->SetPaused(false);
    }
  }
}

POINT Input::InputSystem::GetMousePosition()
{
  return currentMousePosition;
}

void Input::InputSystem::ClearPivot()
{
  pivot = false;
}

void Input::InputSystem::CenterMouse()
{
  SetCursorPos(center.x, center.y);
}

void Input::InputSystem::SetMousePivot(bool val)
{
  pivot = val;
  pivotPoint = currentMousePosition;
}

// set a key state to active
void Input::InputSystem::onKeyTriggered(int keycode)
{
  // key is held down
  keystates[keycode] = true;
}

// reset a keystate
void Input::InputSystem::onKeyRelease(int keycode)
{
  keystates[keycode] = false;
}

void Input::InputSystem::onMouseScroll(int param)
{
  scrollDelta = GET_WHEEL_DELTA_WPARAM(param);
}

void Input::InputSystem::onMouseClick(int param)
{
  keystates[param] = true;
}

// return whether or not a key is being held down
bool Input::InputSystem::keyDown(int key)
{
  return keystates[key];
}

// check if a key was triggered
bool Input::InputSystem::keyTriggered(int key)
{
  return keystates[key] && !previousKeystates[key];
}

int Input::InputSystem::getMouseScrollDelta()
{
  return scrollDelta;
}

bool Input::InputSystem::keyReleased(int key)
{
  return previousKeystates[key] && !keystates[key];
}

void Input::InputSystem::setFocus(bool focus)
{
  focused = focus;
}
