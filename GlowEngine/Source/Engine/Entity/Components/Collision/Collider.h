/*
/
// filename: Collider.h
// author: Callen Betts
// brief: defines the Collider class, holds vertices and Collider data
/
*/

#pragma once
#include "Engine/Entity/Components/Component.h"
#include <set>

namespace Components
{

  class Collider : public Component
  {

  public:

    // the logic to check if we are colliding
    virtual bool isColliding(const Components::Collider* other) = 0;
    // while we are colliding
    virtual void onCollide(const Components::Collider* other) {};
    // when we touch for the first time
    virtual void onFirstCollide(const Components::Collider* other) {};
    // when we leave the collision
    virtual void onLeaveCollide(const Components::Collider* other) {};
    virtual void update();

    // calls the function callback after some checks
    void leaveCollision(const Collider* other);
    // calls the on first collide callback after some checks
    void updateCollision(const Collider* other);

    // render calls respective draw function
    void render();
    virtual void renderDebug() {};

    // calculate the scale, original scale, and vertices of the parent mesh if it exists
    // this must be called at least once for accurate debug drawing and collisions
    void calculateScale();
    void CalculateMeshScale(Vector3D hitboxSize);

    // general getters
    bool isColliding();
    bool hasCollided();
    bool isDirty();
    bool autoSizeEnabled();
    bool isStatic();
    Vector3D getHitboxSize();
    Vector3D getMeshScale();
    
    const std::set<const Components::Collider*>& getCollidingObjects();

    // general setters
    void setColliding(bool val);
    void setCollided(bool val);
    void setAutoSize(bool val);
    void setDirty(bool val);
    void setStatic(bool val);

  protected:

    // flags for collision
    bool colliding = false;
    bool collided = false;
    bool autoSize = true;
    bool dirty = true;

    // static colliders are not checked against any other static colliders
    bool colliderIsStatic = false;

    // debug drawing 
    std::set<const Components::Collider*> collidingObjects;

    // scale of the collider and original mesh scale
    Vector3D scale;
    Vector3D meshScale;
  };

}