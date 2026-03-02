/*
/
// filename: Sprite3D.h
// author: Callen Betts
// brief: defines 3D sprite class for use with models
/
*/

#pragma once
#include "Engine/Entity/Components/Component.h"

namespace Graphics
{
  class Renderer;
}

namespace Textures
{
  class Texture;
}

namespace Components
{

  class Sprite3D : public Component
  {

  public:

    // constructors
    Sprite3D(const std::string modelName, const std::string textureName = "");
    Sprite3D();
    Sprite3D(const Sprite3D& other);

    Sprite3D* clone();
    virtual void load(const nlohmann::json&);
    virtual void CustomLoad(const nlohmann::json);
    virtual void CustomSave(nlohmann::json&) const;

    // initialize
    void init();
    // render - this calls model->render() as well
    void render();
    // draw outline
    void DrawOutline();
    // display model to change
    void display();
    // set the texture repeat
    void setTextureRepeat(bool val);

    // set the sprite's model
    void setModel(const std::string modelName);

    // get the model
    Models::Model* getModel();
    
    // set alpha
    void setAlpha(float alpha);
    // get alpha
    float getAlpha();


  private:

    float alpha;
    bool repeatTexture = false;

    Models::Model* model;
    Graphics::Renderer* renderer;

  };

}