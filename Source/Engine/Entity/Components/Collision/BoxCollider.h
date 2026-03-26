/*
/
// filename: BoxCollider.h
// author: Callen Betts
// brief: defines the BoxCollider class, holds vertices and BoxCollider data
/
*/

#pragma once
#include "Engine/Entity/Components/Component.h"
#include "Engine/Entity/Components/Collision/Collider.h"

namespace Models { class Model; }

namespace Components
{

  class BoxCollider : public Collider
  {

  public:

    BoxCollider(Vector3D newScale = { 1,1,1 }, bool isStatic = true, bool autoResize = true);
    BoxCollider(const BoxCollider& other);

    BoxCollider* clone() { return new Components::BoxCollider(*this); };

    void init();
    virtual void renderDebug();

    virtual bool isColliding(const Components::Collider*) override;
    virtual void onCollide(const Components::Collider* other);

    void setHitboxSize(Vector3D hitboxSize);

    bool isAABBColliding(const BoxCollider& other);

  };

}