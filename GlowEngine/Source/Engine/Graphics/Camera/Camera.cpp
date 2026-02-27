/*
/
// filename: Camera.cpp
// author: Callen Betts
// brief: implements Camera.h
/
*/

#include "stdafx.h"
#include "Camera.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/Window/Window.h"
#include "Engine/Graphics/UI/Editor/GameWindow/GameWindow.h"

// initialize the position and coordinate system of camera
Visual::Camera::Camera(Graphics::Renderer* renderEngine)
  :
  fov(200.f),
  yaw(0),
  pitch(0),
  cameraSpeed(25),
  mouseSensitivity(0.1f)
{
  position = { 0,0,0,1.f };
  rotation = 0;
  viewMatrix = {};
  perspectiveMatrix = {};
  targetPosition = {0,0,1};
  target = nullptr;
  upDirection = { 0,1,0 };
  right = { 0,0,0 };
  forward = { 0,0,1 };
  viewDistance = 250;
  height = 3; // how high we feel 
  engine = EngineInstance::getEngine();
  renderer = renderEngine;
  windowHandle = engine->getWindowHandle();
  input = engine->getInputSystem();
  window = engine->getWindow();
  windowHeight = window->getHeight();
  windowWidth = window->getWidth();
  aspectRatio = static_cast<float>(windowWidth / windowHeight);
  locked = true;
  name = "Camera";

  AddVariable(CreateVariable("Camera Speed", &cameraSpeed));
  AddVariable(CreateVariable("Mouse Sensitivity", &mouseSensitivity));
  AddVariable(CreateVariable("Maximum View Distance", &viewDistance));
  AddVariable(CreateVariable("Eye Level", &height));
  AddVariable(CreateVariable("FOV", &fov));
}

void Visual::Camera::update()
{
  // camera doesn't update unless we're focused on the game window
  if (engine->GameWindowIsFocused())
  {
    // update our camera controller - this handles input and calculates our forward direction, right, and target
    if (engine->isPlaying())
    {
      // game camera controller
      GameCameraController();
    }
    else
    {
      // scene camera controller
      SceneCameraController();
    }
  }

  // update the aspect ratio every frame
  aspectRatio = (float)windowWidth / windowHeight;

  // make sure we cannot look behind us
  pitch = max(-89.0f, min(89.0f, pitch));

  // convert the yaw and pitch to a direction vector
  forward = DirectX::XMVectorSet(
    cos(DirectX::XMConvertToRadians(yaw)) * cos(DirectX::XMConvertToRadians(pitch)),
    sin(DirectX::XMConvertToRadians(pitch)),
    sin(DirectX::XMConvertToRadians(yaw)) * cos(DirectX::XMConvertToRadians(pitch)),
    0.0f);

  // calculate the forward direction - this is where our camera is "looking"
  forward = DirectX::XMVector3Normalize(forward);

  // calculate the right vector from us using the global up direction (0,-1,0)
  right = DirectX::XMVector3Cross({ 0,-1,0 }, forward);
  right = DirectX::XMVector3Normalize(right);

  targetPosition = DirectX::XMVectorAdd(position, forward);

  // Update the camera's up vector
  upDirection = DirectX::XMVector3Cross(forward, right);
  upDirection = DirectX::XMVector3Normalize(upDirection);

  // Update the perspective matrix
  perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, 1.0f, viewDistance);

  // Update the view matrix
  viewMatrix = DirectX::XMMatrixLookAtLH(position, targetPosition, upDirection);

  // Update the renderer's view and perspective matrices
  renderer->updateObjectBufferCameraMatrices();
}

// game controller
void Visual::Camera::GameCameraController()
{
  // multiply our camera speed by delta time
  float dt = engine->getDeltaTime();

  // get the mouse delta so we can look around
  float deltaX = input->getMouseDelta().x;
  float deltaY = input->getMouseDelta().y;

  // update our target position given a target entity
  if (target)
  {
    Vector3D targetPos = getComponentOfType(Transform, target)->getPosition();
    position = DirectX::XMVectorSet(targetPos.x, targetPos.y + height, targetPos.z, 1.0f);
  }

  // have our camera follow our mouse if we're not paused
  if (!engine->IsPaused())
  {
    yaw -= deltaX * mouseSensitivity;
    pitch -= deltaY * mouseSensitivity;
  }
}

// scene camera controller
void Visual::Camera::SceneCameraController()
{
  // multiply our camera speed by delta time
  float dt = engine->getDeltaTime();

  // get the mouse delta so we can look around
  float deltaX = input->getMouseDelta().x;
  float deltaY = input->getMouseDelta().y;
  POINT mousePos = input->GetMousePosition();

  // if we are in editor mode, we can use WASD SHIFT+SPACE to directly set our position
  if (!engine->isPlaying())
  {
    // check for mouse wheel scrolling to increase cameraspeed
    if (Input::InputSystem::MouseScrollDown())
      cameraSpeed -= 2.f;

    if (Input::InputSystem::MouseScrollUp())
      cameraSpeed += 2.f;

    cameraSpeed = std::clamp(cameraSpeed, 5.f, 75.f);

    // get our camera move speed
    float camSpd = cameraSpeed * dt;
    DirectX::XMVECTOR finalPosition = position;

    if (Input::InputSystem::KeyDown('W'))
      finalPosition = DirectX::XMVectorAdd(finalPosition, DirectX::XMVectorScale(forward, camSpd));

    if (Input::InputSystem::KeyDown('S'))
      finalPosition = DirectX::XMVectorAdd(finalPosition, DirectX::XMVectorScale(forward, -camSpd));

    if (Input::InputSystem::KeyDown('A'))
      finalPosition = DirectX::XMVectorAdd(finalPosition, DirectX::XMVectorScale(right, camSpd));

    if (Input::InputSystem::KeyDown('D'))
      finalPosition = DirectX::XMVectorAdd(finalPosition, DirectX::XMVectorScale(right, -camSpd));

    if (Input::InputSystem::KeyDown(VK_SHIFT))
      finalPosition = DirectX::XMVectorAdd(finalPosition, { 0,-camSpd,0 });

    if (Input::InputSystem::KeyDown(VK_SPACE))
      finalPosition = DirectX::XMVectorAdd(finalPosition, { 0,camSpd,0 });

    // pivot by right clicking; we want to be able to pan around our scene
    if (Editor::GameWindow::MouseIsInsideWindow())
    {
      if (Input::InputSystem::KeyDown(MK_RBUTTON))
      {
        yaw -= deltaX * mouseSensitivity;
        pitch -= deltaY * mouseSensitivity;
      }
      if (Input::InputSystem::KeyPressed(MK_RBUTTON))
      {
        input->SetMousePivot(true);
      }
      if (Input::InputSystem::KeyReleased(MK_RBUTTON))
      {
        input->ClearPivot();
      }
    }

    position = finalPosition;
  }
}

// get the view matrix
const Matrix& Visual::Camera::getViewMatrix()
{
  return viewMatrix;
}

// get the perspective matrix
const Matrix& Visual::Camera::getPerspecitveMatrix()
{
  return perspectiveMatrix;
}

const Vector3D Visual::Camera::getForwardVector()
{
  XMVector normalForward = DirectX::XMVector3Normalize(forward);
  return Vector3D(DirectX::XMVectorGetX(normalForward), DirectX::XMVectorGetY(normalForward), DirectX::XMVectorGetZ(normalForward));
}

const Vector3D Visual::Camera::getRightVector()
{
  return Vector3D(DirectX::XMVectorGetX(right), DirectX::XMVectorGetY(right), DirectX::XMVectorGetZ(right));
}

void Visual::Camera::setTarget(Entities::Entity* newTarget)
{
  target = newTarget;
}

void Visual::Camera::SetPosition(const Vector3D pos)
{
  position = { pos.x,pos.y,pos.z };
}

void Visual::Camera::SetRotation(const float yaw_, const float pitch_)
{
  yaw = yaw_;
  pitch = pitch_;
}
