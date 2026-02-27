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

  const std::map<std::string, std::vector<Vertex>>& modelVertices = sprite->getModel()->getModelVertices();
  Components::Transform& transform = *getComponentOfType(Transform, parent);

  // Find the model in the map
  if (sprite->getModel()->getModelNames().empty())
  {
    scale = transform.getScale();
    return;
  }

  std::string modelName = sprite->getModel()->getModelNames()[0];

  auto it = modelVertices.find(modelName);

  const std::vector<Vertex>& vertices = it->second;

  // Initialize min and max with the first vertex
  Vector3D min = Vector3D(vertices[0].x, vertices[0].y, vertices[0].z);
  Vector3D max = Vector3D(vertices[0].x, vertices[0].y, vertices[0].z);

  // Loop through all vertices to find the bounding box
  for (const auto& vertex : vertices) {
    if (vertex.x < min.x) min.x = vertex.x;
    if (vertex.y < min.y) min.y = vertex.y;
    if (vertex.z < min.z) min.z = vertex.z;
    if (vertex.x > max.x) max.x = vertex.x;
    if (vertex.y > max.y) max.y = vertex.y;
    if (vertex.z > max.z) max.z = vertex.z;
  }

  // Calculate the original scale of the mesh
  Vector3D originalScale = max - min;

  // Calculate the scaled size based on the transform's scale
  scale = Vector3D(originalScale.x * transform.getScale().x,
    originalScale.y * transform.getScale().y,
    originalScale.z * transform.getScale().z);

  meshScale = originalScale;

  // Calculate the center of the bounding box
  Vector3D center = (min + max) * 0.5f;

  // Clear the previous vertices
  this->vertices.clear();

  // Define the vertices of the box collider
  Vector3D sc = transform.getScale();
  Vector3D halfScale = sc * 0.5f;
  Vector3D corners[8] = {
      center + Vector3D(-halfScale.x, -halfScale.y, -halfScale.z),
      center + Vector3D(halfScale.x, -halfScale.y, -halfScale.z),
      center + Vector3D(halfScale.x, halfScale.y, -halfScale.z),
      center + Vector3D(-halfScale.x, halfScale.y, -halfScale.z),
      center + Vector3D(-halfScale.x, -halfScale.y, halfScale.z),
      center + Vector3D(halfScale.x, -halfScale.y, halfScale.z),
      center + Vector3D(halfScale.x, halfScale.y, halfScale.z),
      center + Vector3D(-halfScale.x, halfScale.y, halfScale.z)
  };

  // Add the corners to the vertices vector
  for (const auto& corner : corners)
  {
    Vertex v;
    v.x = corner.x;
    v.y = corner.y;
    v.z = corner.z;

    this->vertices.push_back(v);
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

const std::vector<Vertex>& Components::Collider::getVertices()
{
  return vertices;
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
