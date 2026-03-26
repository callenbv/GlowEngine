/*
/
// filename: Physics.cpp
// author: Callen Betts
// brief: implements Physics.h
/
*/

#include "stdafx.h"
#include "Physics.h"
#include "Engine/GlowEngine.h"
#include "Engine/Math/Lerp.h"

REGISTER_COMPONENT(Physics);

Components::Physics::Physics()
{
  maxVelocity = 25.f;
  acceleration = { 0,0,0 };
  velocity = { 0,0,0 };
  gravity = 35.f;
  targetVelocity = { 0,0,0 };
  init();
}

Components::Physics::Physics(const Physics& other) : Component(other)
{
  gravity = other.gravity;
  maxVelocity = other.maxVelocity;
  init();
}

void Components::Physics::init()
{
  type = Components::Component::Physics;
  name = "Physics";
  simulation = false;
  priority = 1;

  AddVariable(CreateVariable("Gravity", &gravity));
  AddVariable(CreateVariable("Acceleration", &acceleration));
  AddVariable(CreateVariable("Velocity", &velocity));
  AddVariable(CreateVariable("Maximum Velocity", &maxVelocity));
}

void Components::Physics::setVelocity(Vector3D vel)
{
  velocity = vel;
}

void Components::Physics::setTargetVelocity(Vector3D vel)
{
  targetVelocity = vel;
}

void Components::Physics::setAcceleration(Vector3D acc)
{
  acceleration = acc;
}

void Components::Physics::addTargetVelocity(Vector3D vec)
{
  targetVelocity += vec;
}

void Components::Physics::setVelocityX(float val)
{
  velocity.x = val;
}

void Components::Physics::setVelocityY(float val)
{
  velocity.y = val;
}

void Components::Physics::setVelocityZ(float val)
{
  velocity.z = val;
}

// update velocity and transform
void Components::Physics::update()
{
  // anchored objects don't move
  if (anchored)
    return;

  float dt = EngineInstance::getEngine()->getDeltaTime();
  Components::Transform* transform = getComponentOfType(Transform, parent);

  if (transform)
  {
    Vector3D position = transform->getPosition();

    if (!grounded)
    {
      // Apply gravity to the acceleration
      acceleration.y -= gravity * dt;
    }
    else
    {
      // Reset vertical acceleration and velocity when grounded
      acceleration.y = 0;
      velocity.y = 0;
    }

    // calculate the final position and update old position
    Vector3D finalPosition = position + ((velocity+acceleration) * dt);

    transform->setOldPosition(position);
    transform->setPosition(finalPosition);
  }
}

void Components::Physics::render()
{
}

void Components::Physics::setAnchored(bool val)
{
  anchored = val;
}

