/*
/
// filename: Texture.h
// author: Callen Betts
// brief: defines Texture class
/
*/

#pragma once

namespace Textures
{
  class TextureLibrary;
}

namespace Graphics
{
  class Renderer;
}

namespace Textures
{

  class Texture
  {

  public:

    Texture();
    Texture(std::string name);

    // initialize a texture
    void init();

    // find a texture in the library and set it
    void load(std::string fileName);

    // load the actual texture data
    void createTexture(std::string fileName);

    // create the sprite's texture resource
    void createTextureResource();

    // get the texture resources
    ID3D11Texture2D** getTexture2D() { return &texture2D; }
    ID3D11ShaderResourceView** getTextureView() { return &textureView; }
    D3D11_SUBRESOURCE_DATA& getTextureSubResource() { return subResource; }
    D3D11_TEXTURE2D_DESC& getTextureDesc() { return textureDesc; }

    // get the name
    std::string getName() { return name; }
    // set the name
    void setName(std::string newName) { name = newName; }

    // get base texture properties
    int getHeight() { return height; }
    int getWidth() { return width; }
    unsigned char* getData() { return data; }

  private:

    Textures::TextureLibrary* textureLibrary;
    std::string name;
    unsigned char* data; // texture data

    int width;
    int height;
    int channels; // color channels, rgba

    // renderer
    Graphics::Renderer* renderer;

    // d3d texture object
    ID3D11Texture2D* texture2D;
    ID3D11ShaderResourceView* textureView;
    D3D11_SUBRESOURCE_DATA subResource;
    D3D11_TEXTURE2D_DESC textureDesc;

  };

}