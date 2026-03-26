/*
/
// filename: Physics.h
// author: Callen Betts
// brief: defines the Physics class, holds vertices and Physics data
/
*/

#pragma once
#include "Engine/Entity/Components/Component.h"

namespace Components
{

  class Physics : public Component
  {

  public:

    Physics();
    Physics(const Physics& other);

    Physics* clone() { return new Components::Physics(*this); };

    void init();
    void setVelocity(Vector3D vel);
    void setTargetVelocity(Vector3D vel);
    void setAcceleration(Vector3D acc);

    void setAccelerationY(float val) { acceleration.y = val; }
    void setTargetVelocityY(float val) { targetVelocity.y = val; }
    void setTargetVelocityX(float val) { targetVelocity.x = val; }
    void setTargetVelocityZ(float val) { targetVelocity.z = val; }
    void addTargetVelocity(Vector3D vec);

    void setVelocityX(float val);
    void setVelocityY(float val);
    void setVelocityZ(float val);

    const Vector3D getVelocity() { return velocity; }
    const Vector3D getAcceleration() { return acceleration; }

    void update();

    void render();

    void setAnchored(bool val);
    bool isAnchored() { return anchored; }
    void setGrounded(bool val) { grounded = val; }
    bool isGrounded() { return grounded; }

  private:

    Vector3D velocity;
    Vector3D targetVelocity;
    Vector3D acceleration;

    // anchored means we can't move no matter what
    bool anchored = false;
    bool grounded = false; // we are on the ground

    // this is the gravity constant
    float gravity;
    float maxVelocity = 100.f;

  };

}