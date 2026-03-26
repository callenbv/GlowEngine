/*
/
// filename: Shader.cpp
// author: Callen Betts
// brief: implements Shader.h
/
*/

#include "stdafx.h"
#include "Shader.h"

Shaders::Shader::Shader(ID3D11Device* device_, std::wstring path, ShaderType type)
  :
  shaderType(type),
  device(device_),
  pixelShader(nullptr),
  vertexShader(nullptr),
  filePath(path)
{
  // get the blob data
  HRESULT hr = D3DReadFileToBlob(path.c_str(), &shaderBlob);
  if (FAILED(hr))
  {
    Logger::error("Failed to create shader blob");
  }

  switch (type)
  {
  case ShaderType::Pixel:
    hr = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pixelShader);
    break;

  case ShaderType::Vertex:
    hr = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &vertexShader);
    break;
  }

  if (FAILED(hr))
  {
    Logger::error("Failed to create shader");
  }
}
