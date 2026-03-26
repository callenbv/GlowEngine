/*
/
// filename: Transform.cpp
// author: Callen Betts
// brief: implements Transform.h
/
*/

#include "stdafx.h"
#include "Transform.h"
#include "Engine/GlowEngine.h"

REGISTER_COMPONENT(Transform);

// initialize a new Transform
Components::Transform::Transform()
  :
  position({ 0, 0, 10 }),
  scale({ 1, 1, 1}),
  rotation({ 0,0,0 }),
  dirty(true)
{
  init();
}

// initialize the transform with position, scale, and rotation
Components::Transform::Transform(Vector3D pos_, Vector3D scale_, Vector3D rotation_)
  :
  position(pos_),
  scale(scale_),
  rotation(rotation_)
{
  init();
}

Components::Transform::Transform(const Transform& other) : Component(other)
{
  Vector3D finalPosition = other.getPosition();
  finalPosition += {0, 0, 0};

  scale = other.scale;
  position = finalPosition;
  rotation = other.rotation;

  init();
}

void Components::Transform::init()
{
  type = ComponentType::Transform;
  name = "Transform";
  dirty = true;

  AddVariable(CreateVariable("Position", &position));
  AddVariable(CreateVariable("Scale", &scale));
  AddVariable(CreateVariable("Rotation", &rotation));
}

void Components::Transform::update()
{
  if (dirty)
  {
    recalculateMatrix();
  }
}

// reclaculate the world matrix
void Components::Transform::recalculateMatrix()
{
  // rotation matrix
  float radianFactor = 52.3f;

  Matrix rotationMatrixX = DirectX::XMMatrixRotationX(rotation.x / radianFactor);
  Matrix rotationMatrixY = DirectX::XMMatrixRotationY(rotation.y / radianFactor);
  Matrix rotationMatrixZ = DirectX::XMMatrixRotationZ(rotation.z / radianFactor);
  Matrix rotationMatrix = rotationMatrixX * rotationMatrixY * rotationMatrixZ;

  // scale matrix
  Matrix scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

  // translation matrix
  Matrix translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

  // set the transform matrix
  transformMatrix = scaleMatrix * rotationMatrix * translationMatrix;

  // reset dirty flag
  dirty = false;
}

// if matrix is dirty
bool Components::Transform::isDirty()
{
  return dirty;
}

// get the transform matrix
const Matrix& Components::Transform::getTransformMatrix()
{
  return transformMatrix;
}

Components::Transform* Components::Transform::clone()
{
  Transform* transform = new Transform(*this);
  return transform;
}

