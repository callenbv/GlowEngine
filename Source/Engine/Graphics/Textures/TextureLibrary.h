/*
/
// filename: TextureLibrary.h
// author: Callen Betts
// brief: defines TextureLibrary class
/
*/

#pragma once
#include "Engine/Graphics/Textures/Texture.h"

namespace Textures
{

  class TextureLibrary
  {

  public:

    TextureLibrary();

    // load all the textures in assets folder
    void load(std::string directoryPath = "Assets/Textures");

    // add a texture to the library
    void add(std::string name, Textures::Texture* texture);
    // get a texture from the library
    Textures::Texture* get(std::string name);

    std::map<std::string, Textures::Texture*> textures;

  };

}