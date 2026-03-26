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
#include <filesystem>
#include "Engine/Systems/Parsing/ObjectLoader.h"

namespace fs = std::filesystem;

// add a material to the library
void Materials::MaterialLibrary::add(Materials::Material* material)
{
  if (material)
  {
    // add material if it was valid
    materials[material->getName()] = material;
  }
  else
  {
    // invalid mesh
    Logger::error("Failed to add material " + material->getName() + " to library");
  }
}

// get a mesh given a name
Materials::Material* Materials::MaterialLibrary::get(std::string name)
{
  return materials[name];
}

// load all of our preset materials 
// this includes things like debug materials, etc.
void Materials::MaterialLibrary::load(std::string directoryPath)
{
    if (fs::is_directory(directoryPath) && fs::exists(directoryPath))
    {
        for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
        {
            if (fs::exists(entry) && fs::is_regular_file(entry))
            {
                std::string fileName = entry.path().stem().string();
                std::string filePath = entry.path().string();
                std::string fileType = entry.path().extension().string();

                if (fileType == ".mtl")
                {
                    // Attempt the parse an MTL file 
                    Parse::ObjectLoader* mtlLoader = new Parse::ObjectLoader();
                    bool opened = mtlLoader->open(filePath);

                    if (opened)
                    {
                        Material* mat = new Material();
                        mtlLoader->parseMTLIsolated(mat);
                        Logger::write("Added material " + fileName);
                        add(mat);
                    }
                }
            }
        }
    }
}

