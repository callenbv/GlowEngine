/*
/
// filename: Camera.h
// author: Callen Betts
// brief: defines the camera class for view and projection matrices
/
*/

#pragma once
#include "Engine/Entity/Entity.h"

namespace Graphics
{
  class Renderer;
  class Window;
}

namespace Visual
{

  class Camera : public Entities::Entity
  {

  public:

    Camera(Graphics::Renderer* renderEngine);

    // update the camera properties
    void update();
    // camera controller
    void GameCameraController();
    void SceneCameraController();

    // get the view matrix
    const Matrix& getViewMatrix();
    // get the perspective matrix
    const Matrix& getPerspecitveMatrix();

    // get the forward vector or 'look' direction
    const Vector3D getForwardVector();
    const Vector3D getRightVector();
    // set the target
    void setTarget(Entities::Entity* newTarget);
    // set the position
    void SetPosition(Vector3D pos);
    void SetRotation(float yaw_, float pitch_);

    // get the position
    XMVector getPosition() { return position; }

  private:

    // renderer
    Graphics::Renderer* renderer;
    Graphics::Window* window;
    Engine::GlowEngine* engine;
    Input::InputSystem* input;

    // camera properties
    XMVector position;
    XMVector targetPosition;
    XMVector upDirection;
    XMVector forward;
    XMVector right;
    float rotation;

    // controller properties
    float fov; // field of view
    float viewDistance; // how far we can see
    float yaw;
    float pitch;
    float cameraSpeed;
    float mouseSensitivity;
    float height; // how tall we are

    // window properties
    int windowWidth;
    int windowHeight;
    float aspectRatio;
    HWND windowHandle;

    // camera contains both the view and perspective matrix
    Matrix viewMatrix;
    Matrix perspectiveMatrix;

    // camera follows an entity
    Entities::Entity* target;

  };

}