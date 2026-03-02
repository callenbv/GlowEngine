/*
/
// filename: Model.cpp
// author: Callen Betts
// brief: implements Model.h
/
*/

#include "stdafx.h"
#include "Model.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Systems/Parsing/ObjectLoader.h"
#include "Engine/Entity/Components/Visual/Models/ModelLibrary.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include "Engine/Graphics/Meshes/MeshLibrary.h"
#include "Engine/Graphics/Meshes/Mesh.h"
#include "Engine/Graphics/Materials/Material.h"
#include "Engine/Graphics/Materials/MaterialLibrary.h"

// construct a new base model
Models::Model::Model()
  :
  modelType(ModelType::Cube)
{
  init();
}

// construct a model given a filename
Models::Model::Model(const std::string fileName)
  :
  modelType(ModelType::Cube)
{
  init();
  load(fileName);
}

// initialize the model
void Models::Model::init()
{
  engine = EngineInstance::getEngine();
  renderer = engine->getRenderer(); // models need the renderer to draw
  device = renderer->getDevice();
  deviceContext = renderer->getDeviceContext();
  dirty = true;
}

// parse a .obj file and load its vertex data into the model
// adds the model to the model library if it doesn't already exist
void Models::Model::load(const std::string fileName)
{
  // query the model library to see if we can get the model's data
  Models::ModelLibrary* library = engine->getModelLibrary();
  Models::Model* model = library->get(fileName);

  if (model)
  {
    // if the model is in the library, just swap pointers
    // this is only possible because the model data (vertices, indices) isn't ever dynamically changed
    // if we ever need a new unique model, we will need to perform a deep copy
    *this = *model;
  }
  else // if it is not in the library
  {
    // parse a 3D model
    Parse::ObjectLoader modelData;

    // parse model data
    modelData.open(fileName);
    modelData.parseAssimp(this);
    modelData.close();
  }
}

// add a mesht to the mesh list
void Models::Model::addMesh(Meshes::Mesh* meshToAdd)
{
    meshes.push_back(meshToAdd);
}

// get the meshes from the model
std::vector<Meshes::Mesh*> Models::Model::getMeshes()
{
    return meshes;
}

// set the model's name to lookup in library
void Models::Model::setName(std::string name_)
{
    name = name_;
}

// get the model's name
std::string&::Models::Model::getName()
{
    return name;
}

// assign a material to every mesh in this model
void Models::Model::assignMaterial(Materials::Material* mat)
{
    if (!mat)
        return;

    // assign the material to all mesh subsections
    for (const auto& mesh : meshes)
    {
        for (auto& section : mesh->getMeshSubsections())
        {
            section.materialName = mat->getName();
        }
    }
}

// assign a material from a string name
void Models::Model::assignMaterialFromName(std::string name)
{
    Materials::Material* mat = EngineInstance::getEngine()->getMaterialLibrary()->get(name);
    assignMaterial(mat);
}

// assign a material to a specific mesh subsection
void Models::Model::assignMaterialToSubSection(int mi, int si, std::string name)
{
    if (mi < 0 || si < 0)
        return;

    if (mi >= static_cast<int>(meshes.size()))
        return;

    auto* mesh = meshes[mi];
    if (!mesh)
        return;

    // IMPORTANT: getMeshSubsections() must return a REFERENCE for this to persist.
    // i.e. std::vector<MeshSubsection>& getMeshSubsections();
    auto& sections = mesh->getMeshSubsections();

    if (si >= static_cast<int>(sections.size()))
        return;

    // Only assign if the material actually exists
    Materials::Material* mat =
        EngineInstance::getEngine()->getMaterialLibrary()->get(name);

    if (!mat)
        return;

    sections[si].materialName = mat->getName();
}

// render a model's meshes
void Models::Model::render()
{
    // render each mesh
    for (auto& mesh : meshes)
    {
        mesh->render();
    }
}
