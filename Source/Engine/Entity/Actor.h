/*
/
// filename: Actor.h
// author: Callen Betts
// brief: defines Actor class
/
*/

#pragma once
#include "Engine/Entity/Entity.h"
#include "Engine/Graphics/Lighting/LightBuffer.h"

namespace Entities
{
 
  class Actor : public Entity
  {

  public:

    Actor();
    Actor(const Entity& other);
    virtual ~Actor() = default;
    void init();

    // ** Transform ** //
    void setPosition(Vector3D pos) { transform->setPosition(pos); }
    Vector3D getPosition() { return transform->getPosition(); }

    void setScale(Vector3D newScale) { transform->setScale(newScale); }
    Vector3D getScale() { return transform->getScale(); }

    void setRotation(Vector3D rot) { transform->setRotation(rot); }
    Vector3D getRotation() { return transform->getRotation(); }

    // ** Physics ** //
    void setAcceleraton(Vector3D acc);
    void setVelocity(Vector3D vel);
    void setAnchored(bool val);

    // ** Model ** //
    void setModel(std::string name);

    // ** Texture ** //
    void setTexture(std::string name);
    void setTextureRepeat(bool val);

    // ** Lighting ** //
    void setAsPointLight(bool val);
    void createPointLight();
    void updatePointLight(Vector3D pos, float size, DirectX::XMFLOAT4 color);
    void setLightSize(float size);

    // ** General ** //
    void setName(std::string name) { this->name = name; }

    // ** Collision ** //
    void setHitboxSize(Vector3D size);
    void setStatic(bool val);
    Vector3D getHitboxSize();

  private:

    // lighting properties
    bool isLight;
    PointLight* light;

    // common pointers actors use
    Engine::GlowEngine* engine;
    Graphics::Renderer* renderer;

  };

}