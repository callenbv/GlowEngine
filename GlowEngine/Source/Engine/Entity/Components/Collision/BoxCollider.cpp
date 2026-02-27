/*
/
// filename: Physics.cpp
// author: Callen Betts
// brief: implements Physics.h
/
*/

#include "stdafx.h"
#include "BoxCollider.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/Meshes/MeshLibrary.h"

REGISTER_COMPONENT(BoxCollider);

/// <summary>
/// Constructor override for extra parameters (static, autoresize)
/// </summary>
/// <param name="newScale"></param>
/// <param name="isStatic"></param>
/// <param name="autoResize"></param>
Components::BoxCollider::BoxCollider(Vector3D newScale, bool isStatic, bool autoResize)
{
  colliderIsStatic = isStatic;
  autoSize = autoResize;
  scale = newScale;
  init();
}

/// <summary>
/// Copy constructor
/// </summary>
/// <param name="other"></param>
Components::BoxCollider::BoxCollider(const BoxCollider& other)
{
  autoSize = other.autoSize;
  scale = other.scale;
  meshScale = other.meshScale;
  colliderIsStatic = other.colliderIsStatic;
  init();
}

/// <summary>
/// Initialize the box collider component
/// </summary>
void Components::BoxCollider::init()
{
  type = ComponentType::Collider;
  name = "BoxCollider";
  simulation = false;

  AddVariable(CreateVariable("Hitbox Size", &scale));
  AddVariable(CreateVariable("Static", &colliderIsStatic));
  AddVariable(CreateVariable("Automatically Resize Hitbox", &autoSize));
}

/// <summary>
/// Check if we are colliding with another collider
/// </summary>
/// <param name="other"></param>
/// <returns></returns>
bool Components::BoxCollider::isColliding(const Components::Collider* other)
{
  Components::Transform* otherTransform = getComponentOfType(Transform, other->parent);
  Components::Transform* transform = getComponentOfType(Transform, parent);

  Vector3D position = transform->getPosition();
  Vector3D otherPosition = otherTransform->getPosition();
  const BoxCollider* otherBox = dynamic_cast<const BoxCollider*>(other);

  return isAABBColliding(*otherBox);
}

/// <summary>
/// Check if we are colliding with another box collider
/// </summary>
/// <param name="other"></param>
/// <returns></returns>
bool Components::BoxCollider::isAABBColliding(const BoxCollider& other) 
{
  Components::Transform* otherTransform = getComponentOfType(Transform, other.parent);
  Components::Transform* transform = getComponentOfType(Transform, parent);

  Vector3D position = transform->getPosition();
  Vector3D otherPosition = otherTransform->getPosition();

  Vector3D minA = position - scale * 0.5f;
  Vector3D maxA = position + scale * 0.5f;

  Vector3D otherScale = other.scale;
  Vector3D minB = otherPosition - otherScale * 0.5f;
  Vector3D maxB = (otherPosition + otherScale * 0.5f);

  bool overlapX = (minA.x < maxB.x) && (maxA.x > minB.x);
  bool overlapY = (minA.y <= maxB.y) && (maxA.y > minB.y);
  bool overlapZ = (minA.z < maxB.z) && (maxA.z > minB.z);

  return overlapX && overlapY && overlapZ;
}

/// <summary>
/// Called when we are overlapping with another collider
/// </summary>
/// <param name="other"></param>
void Components::BoxCollider::onCollide(const Components::Collider* other)
{
  // Get the physics and transform components
  Components::Physics* physics = getComponentOfType(Physics, parent);

  if (!physics)
  {
    return;
  }

  Components::Transform* transform = getComponentOfType(Transform, parent);
  Components::Transform* otherTransform = getComponentOfType(Transform, other->parent);
  const BoxCollider* otherBox = dynamic_cast<const BoxCollider*>(other);

  // Get the current position and velocity
  Vector3D currentPosition = transform->getPosition();
  Vector3D oldPosition = transform->getOldPosition();
  Vector3D velocity = physics->getVelocity();

  // Get the positions of both colliding objects
  Vector3D otherPosition = otherTransform->getPosition();

  // Calculate the half-sizes
  Vector3D halfSizeA = scale * 0.5f;
  Vector3D otherScale = otherBox->scale;
  Vector3D halfSizeB = otherScale * 0.5f;

  // Calculate the penetration depths along each axis
  float penetrationX = halfSizeA.x + halfSizeB.x - std::abs(currentPosition.x - otherPosition.x);
  float penetrationY = halfSizeA.y + halfSizeB.y - std::abs(currentPosition.y - otherPosition.y);
  float penetrationZ = halfSizeA.z + halfSizeB.z - std::abs(currentPosition.z - otherPosition.z);

  // Determine the primary collision axis by finding the smallest penetration depth
  float minPenetration = penetrationX;
  Vector3D collisionNormal = Vector3D(1, 0, 0);

  if (penetrationY < minPenetration)
  {
    collisionNormal = Vector3D(0, 1, 0); // Y-axis
    minPenetration = penetrationY;
  }

  if (penetrationZ < minPenetration)
  {
    collisionNormal = Vector3D(0, 0, 1); // Z-axis
    minPenetration = penetrationZ;
  }

  if (minPenetration <= 0) 
    return;

  // Adjust the velocity based on the collision normal
  velocity = velocity - collisionNormal * (velocity.dot(collisionNormal));

  // If the smallest penetration is in the Y-axis, handle grounding and position adjustment
  if (collisionNormal.y == 1 && currentPosition.y > otherPosition.y)
  {
    physics->setGrounded(true);
    velocity.y = 0;

    if (minPenetration > 0)
    {
      currentPosition.y += minPenetration * 0.5f; // Adjust slightly to avoid jitter
      transform->setPosition(currentPosition);
    }
  }
  else if (!physics->isAnchored())
  {
    if (minPenetration > 0)
    {
      currentPosition += collisionNormal * minPenetration * 0.00001f;
      transform->setPosition({oldPosition.x,currentPosition.y,oldPosition.z});
    }
  }

  // Set the adjusted velocity
  physics->setVelocity(velocity);
}

/// <summary>
/// Set the hitbox size of the collider
/// </summary>
/// <param name="hitboxSize"></param>
void Components::BoxCollider::setHitboxSize(Vector3D hitboxSize)
{
  calculateScale();
  meshScale = hitboxSize / getComponentOfType(Transform, parent)->getScale();
  scale = hitboxSize;
  dirty = false;
}

/// <summary>
/// Draw a debug wireframe of the collision box
/// </summary>
void Components::BoxCollider::renderDebug()
{
  Graphics::Renderer* renderer = EngineInstance::getEngine()->getRenderer();
  Meshes::MeshLibrary* lib = EngineInstance::getEngine()->getMeshLibrary();

  renderer->drawSetColor(Color::Red);
  lib->drawBox(this);
  renderer->drawSetColor(Color::Clear);
}