/*
/
// filename: Transform.h
// author: Callen Betts
// brief: defines the Transform class, holds vertices and Transform data
/
*/

#pragma once
#include "Engine/Entity/Components/Component.h"

namespace Components
{

  class Transform : public Component
  {

  public:

    Transform();
    Transform(Vector3D pos_, Vector3D scale_, Vector3D rotation_);
    Transform(const Transform& other);

    // initialize
    void init();
    void update();

    // we want to avoid doing this as much as possible
    void recalculateMatrix();
    // check if dirty flag is set for matrix recalculation
    bool isDirty();
    // get the transform matrix
    const Matrix& getTransformMatrix();

    Components::Transform* clone();

    // set dirty
    void setDirty(bool val) { dirty = val; }

    // position
    void setPosition(Vector3D pos) { position = pos; dirty = true; }
    Vector3D getPosition() { return position; }
    const Vector3D getPosition() const { return position; }

    void setOldPosition(Vector3D pos) { oldPosition = pos; }
    Vector3D getOldPosition() { return oldPosition; }

    // get scale
    Vector3D getScale() { return scale; }
    const Vector3D getScale() const { return scale; }
    void setScale(Vector3D newScale) { scale = newScale; dirty = true; }

    // rotation
    Vector3D getRotation() { return rotation; }
    void setRotation(Vector3D rot) { rotation = rot; dirty = true; }

  private:

    Vector3D position;
    Vector3D oldPosition;
    Vector3D scale;
    Vector3D rotation;

    Matrix transformMatrix;

    bool dirty;

  };
}
