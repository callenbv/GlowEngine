/*
/
// filename: Texture.cpp
// author: Callen Betts
// brief: implements Texture.h
/
*/

#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"
#include "Engine/GlowEngine.h"
#include "Engine/EngineInstance.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"

// default constructor
Textures::Texture::Texture()
{
  init();
}

// overloaded constructor to load an image immediately
Textures::Texture::Texture(std::string texName)
{
  init();
  load(texName);
}

// initialize texture properties
void Textures::Texture::init()
{
  // give textures a pointer to the texture library
  Engine::GlowEngine* engine = EngineInstance::getEngine();
  textureLibrary = engine->getTextureLibrary();
  renderer = engine->getRenderer();
  texture2D = nullptr;
  textureView = nullptr;
  textureDesc = {};
}

// load a texture given a filename 
// this will search the texture library and fetch the resource, or create it if it does not exist
void Textures::Texture::load(std::string fileName)
{
  // get the texture from the library
  Textures::Texture* tex = textureLibrary->get(fileName);
  if (tex)
  {
    *this = *tex;
    return;
  }
}

// create the texture data - we call this whenever we 
void Textures::Texture::createTexture(std::string filePath)
{
  // could not find texture
  data = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (!data)
  {
    // if data was invalid
    Logger::error("Failed to load texture " + filePath);
  }

  // create/update the texture resource
  createTextureResource();
}

// create the directX texture description
void Textures::Texture::createTextureResource()
{
  // setup the texture description and resource
  textureDesc.Width = width;
  textureDesc.Height = height;
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 1;
  textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // this must match the stbi_load format
  textureDesc.SampleDesc.Count = 1;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  textureDesc.CPUAccessFlags = 0;

  // set the system memory
  subResource.pSysMem = data;
  subResource.SysMemPitch = width * 4;
  subResource.SysMemSlicePitch = 0;

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = textureDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MostDetailedMip = 0;
  srvDesc.Texture2D.MipLevels = 1;

  // create the texture
  renderer->getDevice()->CreateTexture2D(&textureDesc, &subResource, &texture2D);
  if (texture2D)
  {
    renderer->getDevice()->CreateShaderResourceView(texture2D, &srvDesc, &textureView);
  }
}
