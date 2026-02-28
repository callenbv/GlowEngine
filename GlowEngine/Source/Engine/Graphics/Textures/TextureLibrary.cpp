/*
/
// filename: TextureLibrary.cpp
// author: Callen Betts
// brief: implements TextureLibrary.h
/
*/

#include "stdafx.h"
#include "TextureLibrary.h"
#include <filesystem>

namespace fs = std::filesystem;

Textures::TextureLibrary::TextureLibrary()
{

}

// load all the textures in the assets folder
void Textures::TextureLibrary::load(std::string directoryPath)
{
  // 1. check if the directory exists and if it is a directory
  if (fs::is_directory(directoryPath) && fs::exists(directoryPath))
  {
    // 2. loop through each folder within the directory
    for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
    {
      if (fs::exists(entry) && fs::is_regular_file(entry))
      {
        // 3. texture detected, add it to the library
        std::string fileName = entry.path().stem().string();
        std::string filePath = entry.path().string();
        std::string fileType = entry.path().extension().string();
        // 4. check that the file is an image type (.png and .jpg support)
        if (fileType == ".png" || fileType == ".jpg")
        {
          // 5. create the texture and load the data
          Textures::Texture* newTexture = new Textures::Texture();
          newTexture->createTexture(filePath);
          add(fileName, newTexture);
        }
      }
    }
  }
}

// add a texture to the library
void Textures::TextureLibrary::add(std::string name, Textures::Texture* texture)
{
  // add the texture to the library
  texture->setName(name);
  if (texture)
  {
    textures[name] = texture;
    Logger::write("Added new texture " + name + " to library");
  }
  else
  {
    Logger::error("Failed to add " + name + " to texture library");
  }
}

// get a texture from the library
Textures::Texture* Textures::TextureLibrary::get(std::string name)
{
    Textures::Texture* texture = textures[name];
    // return the final texture
    return texture;
}