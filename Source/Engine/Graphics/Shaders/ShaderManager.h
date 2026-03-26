/*
/
// filename: ShaderManager.h
// author: Callen Betts
// brief: defines ShaderManager class
/
*/

#pragma once

namespace Shaders
{
  class Shader;

  class ShaderManager
  {

  public:

    // load all the shaders
    ShaderManager(ID3D11Device* device_, ID3D11DeviceContext* context_);

    // create a new shader
    Shader* createShader(std::string name, ShaderType type);
    // get shader class object
    Shader* get(std::string name);
    // setup the shader manager
    void load();

    // setup the input layout
    void setup();

    // get respective pixel and vertex shader directX objects
    ID3D11PixelShader* getPixelShader(std::string name);
    ID3D11VertexShader* getVertexShader(std::string name);

  private:

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    std::map<std::string, Shader*> shaders;

  };
}