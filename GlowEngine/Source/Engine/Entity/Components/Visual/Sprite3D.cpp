/*
/
// filename: Sprite3D.cpp
// author: Callen Betts
// brief: implements Sprite3D.h
/
*/

#include "stdafx.h"
#include "Sprite3D.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Graphics/Textures/Texture.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include "Engine/EngineInstance.h"
#include "Engine/Graphics/Lighting/Shadows/ShadowSystem.h"
#include "Engine/Entity/Components/Visual/Models/ModelLibrary.h"
#include "Engine/Graphics/Materials/Material.h"
#include "Engine/Graphics/Materials/MaterialLibrary.h"

REGISTER_COMPONENT(Sprite3D);

// overloaded constructor to take in a model and a texture
Components::Sprite3D::Sprite3D(const std::string modelName, const std::string textureName)
  : Component(),
  renderer(nullptr)
{
  init();
}

// base Sprite3D constructor to give pointers to renderer
Components::Sprite3D::Sprite3D()
  : Component(),
  renderer(nullptr)
{
  init();
}

Components::Sprite3D::Sprite3D(const Sprite3D& other) : Component(other)
{
  init();
  setModel(other.model->getName());
}

Components::Sprite3D* Components::Sprite3D::clone()
{
  return new Sprite3D(*this);
}

void Components::Sprite3D::load(const nlohmann::json& data) 
{
}

/// <summary>
/// Refreshes the model after being loaded
/// </summary>
void Components::Sprite3D::CustomLoad(const nlohmann::json saveData)
{
  if (model)
  {
    // Assign the model
    if (saveData.contains("Model")) setModel(saveData["Model"]["value"]);

    // Assign material
    if (saveData.contains("Materials") && saveData["Materials"].is_array())
    {
        for (const auto& entry : saveData["Materials"])
        {
            int mi = entry.value("mesh", -1);
            int si = entry.value("section", -1);
            std::string name = entry.value("name", "");

            if (mi < 0 || si < 0 || name.empty())
                continue;

            model->assignMaterialToSubSection(mi, si, name); // you implement this
        }
    }
  }
}

/// <summary>
/// Save things like textures and meshes
/// </summary>
/// <param name=""> The json object </param>
void Components::Sprite3D::CustomSave(nlohmann::json& saveData) const
{
    saveData["Materials"] = nlohmann::json::array();

    int mi = 0;
    for (const auto& mesh : model->getMeshes())
    {
        int si = 0;
        for (const auto& section : mesh->getMeshSubsections())
        {
            saveData["Materials"].push_back({
                {"mesh", mi},
                {"section", si},
                {"name", section.materialName}
                });
            si++;
        }
        mi++;
    }
}

// initialize the Sprite3D component
void Components::Sprite3D::init()
{
  model = new Models::Model("Cube");
  type = Components::Component::Sprite3D;
  name = "Sprite3D";
  Engine::GlowEngine* engine = EngineInstance::getEngine();
  renderer = engine->getRenderer();

  AddVariable(CreateVariable("Model", &(model->getName())));
  AddVariable(CreateVariable("Repeat Texture", &repeatTexture));
}

// render a Sprite3D's model
void Components::Sprite3D::render()
{
    // set transform constant buffer
    Components::Transform* transform = getComponentOfType(Transform, parent);
    if (!transform)
    {
        return;
    }

    // update the constant buffer's world matrix
    renderer->updateObjectBufferWorldMatrix(transform->getTransformMatrix());

    // bind the constant buffer and update sub resource
    renderer->updateObjectBuffer();

    // texture repeat
    for (const auto& mesh : model->getMeshes())
    {
        for (const auto& section : mesh->getMeshSubsections())
        {
            EngineInstance::getEngine()->getMaterialLibrary()->get(section.materialName)->repeatTexture = repeatTexture;
        }
    }

    // render our model
    model->render();

    // reset UV scale after drawing
    renderer->SetUVScale(1, 1);
}

// draw the outline of this sprite 
void Components::Sprite3D::DrawOutline()
{
  if (parent->IsSelected())
  {
    Components::Transform* transform = getComponentOfType(Transform, parent);
    renderer->DrawSetOutline(Color::Outline);
    Vector3D outlineScale = transform->getScale() * 1.02f;
    Vector3D oldScale = transform->getScale();
    transform->setScale(outlineScale);
    transform->recalculateMatrix();
    renderer->updateObjectBufferWorldMatrix(transform->getTransformMatrix());
    renderer->updateObjectBuffer();
    transform->setScale(oldScale);
    model->render();
    renderer->DrawSetOutline(Color::Clear);
  }
}

void Components::Sprite3D::display()
{
  Models::ModelLibrary* lib = EngineInstance::getEngine()->getModelLibrary();
  Textures::TextureLibrary* tex = EngineInstance::getEngine()->getTextureLibrary();
  Materials::MaterialLibrary* matLib = EngineInstance::getEngine()->getMaterialLibrary();

  // Assuming `currentModel` is a member variable that holds the name of the current model
  static std::string currentModel = "";
  static std::string currentMaterial = "";

  // Assign Models
  if (ImGui::TreeNode("Models"))
  {
    for (const auto& modelEntry : lib->models)
    {
      const std::string& modelName = modelEntry.first;
      bool isSelected = (modelName == currentModel);

      if (ImGui::Selectable(modelName.c_str(), isSelected))
      {
        // Update the current model when the selectable item is clicked
        currentModel = modelName;

        // Update your model here, e.g., loading the new model into your Sprite3D component
        setModel(modelName); // Assuming setModel is a method to update the model
      }

      // Keep the selection highlighted if it matches the current model
      if (isSelected)
      {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::TreePop();
  }

  // Assign materials
  if (ImGui::TreeNode("Materials"))
  {
      for (const auto& mat : matLib->getMaterials())
      {
          const std::string& name = mat.first;
          bool isSelected = (name == currentMaterial);

          // Assign the material
          if (ImGui::Selectable(name.c_str(), isSelected))
          {
              currentMaterial = name;
              model->assignMaterial(matLib->get(currentMaterial));
          }

          if (isSelected)
          {
              ImGui::SetItemDefaultFocus();
          }
      }
      ImGui::TreePop();
  }

}

// set this model given a name
void Components::Sprite3D::setModel(const std::string modelName)
{
  model->load(modelName);
}

// get the Sprite3D's model
Models::Model* Components::Sprite3D::getModel()
{
  return model;
}

// set alpha
void Components::Sprite3D::setAlpha(float newAlpha)
{
  alpha = newAlpha;
}

// get alpha
float Components::Sprite3D::getAlpha()
{
  return alpha;
}

void Components::Sprite3D::setTextureRepeat(bool val)
{
    repeatTexture = val;
}
