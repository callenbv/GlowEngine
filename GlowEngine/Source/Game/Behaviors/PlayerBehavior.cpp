/*
/
// filename: PlayerBehavior.cpp
// author: Callen Betts
// brief: implements PlayerBehavior.h
/
*/

#include "stdafx.h"
#include "PlayerBehavior.h"
#include "Engine/Graphics/Camera/Camera.h"
#include "Engine/EngineInstance.h"
#include "Engine/GlowEngine.h"

// create a PlayerBehavior
REGISTER_BEHAVIOR(PlayerBehavior);

Game::PlayerBehavior::PlayerBehavior() : Behavior()
{
  type = Components::Component::ComponentType::Behavior;
  moveSpeed = 20.f;
  jumpSpeed = 25.f;
  god = false;
  name = "PlayerBehavior";

  AddVariable(CreateVariable("Movement Speed", &moveSpeed));
  AddVariable(CreateVariable("Jump Height", &jumpSpeed));
  AddVariable(CreateVariable("God Mode", &god));
}

// move our player
void Game::PlayerBehavior::update()
{
  // get the components we need to move
  Components::Transform* transform = getComponentOfType(Transform, parent);
  Components::Physics* physics = getComponentOfType(Physics, parent);

  if (!physics || !transform)
    return;

  // moves based on the direction we're looking
  Visual::Camera* camera = engine->getCamera();
  Vector3D forward = camera->getForwardVector();
  Vector3D right = camera->getRightVector();

  forward.y = 0;
  forward.normalize();
  camera->setTarget(parent);

  Vector3D forwardVelocity = forward * moveSpeed;
  Vector3D rightVelocity = right * moveSpeed;

  Vector3D targetVelocity = { 0,0,0 };

  if (input->keyDown('W'))
  {
    targetVelocity.x += forwardVelocity.x;
    targetVelocity.z += forwardVelocity.z;
  }
  if (input->keyDown('A'))
  {
    targetVelocity.x += rightVelocity.x;
    targetVelocity.z += rightVelocity.z;
  }
  if (input->keyDown('S'))
  {
    targetVelocity.x -= forwardVelocity.x;
    targetVelocity.z -= forwardVelocity.z;
  }
  if (input->keyDown('D'))
  {
    targetVelocity.x -= rightVelocity.x;
    targetVelocity.z -= rightVelocity.z;
  }

  if (input->keyDown(VK_SPACE) && physics->isGrounded())
  {
    physics->setVelocityY(jumpSpeed);
    transform->setPosition(transform->getPosition() + Vector3D(0.f, 0.1f, 0.f));
  }

  // for diagonal movement
  if (targetVelocity.x != 0 || targetVelocity.z != 0)
  {
    float length = std::sqrt(targetVelocity.x * targetVelocity.x + targetVelocity.z * targetVelocity.z);
    targetVelocity.x /= length;
    targetVelocity.z /= length;

    targetVelocity.x *= moveSpeed;
    targetVelocity.z *= moveSpeed;
  }

  physics->setVelocityX(targetVelocity.x);
  physics->setVelocityZ(targetVelocity.z);
}

