/*
/
// filename: ModelLibrary.cpp
// author: Callen Betts
// brief: implements ModelLibrary.h
/
*/

#include "stdafx.h"
#include "ModelLibrary.h"
#include "Engine/Systems/Parsing/ObjectLoader.h"
#include <filesystem>

namespace fs = std::filesystem;

Models::ModelLibrary::ModelLibrary()
{
}

// loop through our assets and load all of our 
void Models::ModelLibrary::load(std::string directoryPath)
{
  // 1. check if the directory exists and if it is a directory
  if (fs::is_directory(directoryPath) && fs::exists(directoryPath))
  {
    // 2. loop through each folder within the directory
    for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
    {
      if (fs::exists(entry) && fs::is_regular_file(entry))
      {
        // 3. model detected, add it to the library
        std::string fileName = entry.path().stem().string();
        std::string filePath = entry.path().string();
        std::string fileType = entry.path().extension().string();
        // 4. check that the file is of type ".obj"
        if (fileType == ".obj" || fileType == ".fbx")
        {
          add(fileName, new Models::Model(filePath));
        }
      }
    }
  }
}

// add a model to the library
void Models::ModelLibrary::add(std::string name, Models::Model* model)
{
  if (model)
  {
    model->setName(name);
    models[name] = model;
  }
  Logger::write("Added new model " + name + " to library");
}

// get a model from the library
Models::Model* Models::ModelLibrary::get(std::string name)
{
  auto it = models.find(name);
  if (it != models.end())
  {
    return it->second;
  }
  else
  {
    // Handle the case where the model is not found
    return nullptr; // Or handle it appropriately
  }
}
