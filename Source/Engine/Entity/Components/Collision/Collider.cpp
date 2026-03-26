/*
/
// filename: Collider.cpp
// author: Callen Betts
// brief: Implements Collider.h
/
*/

#include "stdafx.h"
#include "Collider.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"

void Components::Collider::update()
{
  // if we dont have a physics object, auto-mark as a static collider
  // if we do, and it's not anchored, then we are a moving collider
  Components::Physics* physics = getComponentOfType(Physics, parent);
  Components::Transform* transform = getComponentOfType(Transform, parent);

  // if we have no physics, no point in being not static
  if (!physics)
  {
    colliderIsStatic = true;
  }
}

// when we leave a collision, we erase the other collider from the active list
// we also call our collider callback for anything specific
void Components::Collider::leaveCollision(const Components::Collider* other)
{
  // erase the reference to the other collider since we've stopped colliding
  collidingObjects.erase(other);
  collided = false;
  colliding = false;

  // if we have a physics component, then we want to do some falling logic
  Components::Physics* physics = getComponentOfType(Physics, parent);
  Components::Transform* transform = getComponentOfType(Transform, parent);
  Components::Transform* otherTransform = getComponentOfType(Transform, other->parent);

  if (physics)
  {
    bool grounded = false;
    for (auto* collider : collidingObjects)
    {
      Vector3D collisionNormal = transform->getPosition() - otherTransform->getPosition();
      collisionNormal.normalize();
      if (collisionNormal.y > 0)
      {
        grounded = true;
        break;
      }
    }
    physics->setGrounded(grounded);
  }

  // componenet function callback
  onLeaveCollide(other);
}

// when we enter a collision, we add the collider to its list of colliding objects
// we then call its resolution callback for entering and then colliding
void Components::Collider::updateCollision(const Collider* other)
{
  // when we enter a collision, this is triggered only once
  if (!collided)
  {
    collided = true;
    collidingObjects.insert(other);
    onFirstCollide(other);
  }

  // call the normal collision resolution callback
  onCollide(other);
  colliding = true;
}

// call debug render if we have it enabled
void Components::Collider::render()
{
  Graphics::Renderer* renderer = EngineInstance::getEngine()->getRenderer();
  Components::Transform* transform = getComponentOfType(Transform, parent);

  // recalculate the real scale of the mesh when our transform changes
  // this can be modified to only be done if the scale changes in the future
  if (transform->isDirty() || dirty)
  {
    // autosize will calculate the bounding box dynamically
    if (autoSize)
    {
      calculateScale();
      meshScale = scale / transform->getScale();
    }
    else
    {
      // we just want to change our mesh scale to our actual scale and update vertices
      CalculateMeshScale(scale);
    }
    dirty = false;
  }

  if (renderer->isDebugMode())
  {
    renderer->setRasterizerFillMode(D3D11_FILL_WIREFRAME);
    renderDebug();
    renderer->setRasterizerFillMode(D3D11_FILL_SOLID);
  }
}

// our scalar value works for collisions at small values, but as we have larger and larger objects,
// we need to use the original scale of our mesh instead for accurate collisions
void Components::Collider::calculateScale()
{
  // find out vertices
  Components::Sprite3D* sprite = getComponentOfType(Sprite3D, parent);

  if (!sprite)
    return;

  const std::vector<Vertex> vertices = sprite->getModel()->allVertices;
  Components::Transform& transform = *getComponentOfType(Transform, parent);

  // Calculate the scale of the 
  if (vertices.empty())
  {
    scale = transform.getScale();
    return;
  }



  Components::BoundingBox* boundingBox = getComponentOfType(BoundingBox, parent);
  dirty = getComponentOfType(Transform, parent)->isDirty();
}

void Components::Collider::CalculateMeshScale(Vector3D hitboxSize)
{
  calculateScale();
  meshScale = hitboxSize / getComponentOfType(Transform,parent)->getScale();
  scale = hitboxSize;
  dirty = false;
}


bool Components::Collider::isColliding()
{
  return colliding;
}

bool Components::Collider::hasCollided()
{
  return collided;
}

bool Components::Collider::isDirty()
{
  return dirty;
}

bool Components::Collider::autoSizeEnabled()
{
  return autoSize;
}

bool Components::Collider::isStatic()
{
  return colliderIsStatic;
}

Vector3D Components::Collider::getHitboxSize()
{
  return scale;
}

Vector3D Components::Collider::getMeshScale()
{
  return meshScale;
}

const std::set<const Components::Collider*>& Components::Collider::getCollidingObjects()
{
  return collidingObjects;
}

void Components::Collider::setColliding(bool val)
{
  colliding = val;
}

void Components::Collider::setCollided(bool val)
{
  collided = val;
}

void Components::Collider::setAutoSize(bool val)
{
  autoSize = val;
}

void Components::Collider::setDirty(bool val)
{
  dirty = val;
}

void Components::Collider::setStatic(bool val)
{
  colliderIsStatic = val;
}
