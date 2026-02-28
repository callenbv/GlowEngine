/*
/
// filename: MeshLibrary.cpp
// author: Callen Betts
// brief: implements MeshLibrary.h
/
*/

#include "stdafx.h"
#include "MaterialLibrary.h"
#include "Engine/Graphics/Materials/Material.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Entity/Components/Visual/Models/ModelLibrary.h"
#include "Engine/GlowEngine.h"

// add a material to the library
void Materials::MaterialLibrary::add(Materials::Material* material)
{
  if (material)
  {
    // add material if it was valid
    materials[material->getName()] = material;
    Logger::write("Added new mesh "+ material->getName());
  }
  else
  {
    // invalid mesh
    Logger::error("Failed to add mesh " + material->getName() + " to library");
  }
}

// get a mesh given a name
Materials::Material* Materials::MaterialLibrary::get(std::string name)
{
  return materials[name];
}

// load all of our preset materials (no-op)
void Materials::MaterialLibrary::load()
{

}

