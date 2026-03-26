/*
/
// filename: Shader.h
// author: Callen Betts
// brief: defines Shader class
/
*/

#pragma once

namespace Shaders
{

  // base shader class
  class Shader
  {

  public:

    Shader(ID3D11Device* device, std::wstring path, ShaderType type);

    ShaderType getType() { return shaderType; }

    ID3D11PixelShader* getPixelShader() { return pixelShader; }
    ID3D11VertexShader* getVertexShader() { return vertexShader; }
    ID3DBlob* getBlob() { return shaderBlob; }

  protected:

    std::wstring filePath;
    ShaderType shaderType;
    ID3DBlob* shaderBlob;
    ID3D11Device* device; // for making shaders
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

  };

}