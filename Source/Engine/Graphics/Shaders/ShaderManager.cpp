/*
/
// filename: ShaderManager.cpp
// author: Callen Betts
// brief: implements ShaderManager.h
/
*/

#include "stdafx.h"
#include "ShaderManager.h"
#include "Shader.h"
#include <filesystem>

namespace fs = std::filesystem;

// load all of our shaders
Shaders::ShaderManager::ShaderManager(ID3D11Device* device_, ID3D11DeviceContext* context_)
  :
  device(device_),
  context(context_)
{

}

// creates a shader and adds it to the list 
Shaders::Shader* Shaders::ShaderManager::createShader(std::string name, ShaderType type)
{
  std::string fileName = name + ".cso";

  wchar_t buffer[MAX_PATH];
  GetModuleFileNameW(nullptr, buffer, MAX_PATH);
  std::wstring executablePath = buffer;

  fs::path executableDir = executablePath;
  fs::path shaderDirectory = executableDir.parent_path();
  std::wstring filePath = (shaderDirectory / fileName);

  Shader* shader = new Shader(device, filePath, type);
  shaders[name] = shader;

  return shader;
}

Shaders::Shader* Shaders::ShaderManager::get(std::string name)
{
  return shaders[name];
}

// define shaders you want to load in here
void Shaders::ShaderManager::load()
{
  createShader("VertexShader", ShaderType::Vertex);
  createShader("PixelShader", ShaderType::Pixel);
  createShader("UnlitPixelShader", ShaderType::Pixel);
}

ID3D11VertexShader* Shaders::ShaderManager::getVertexShader(std::string name)
{
  Shaders::Shader* shader = get(name);
  ID3D11VertexShader* vertexShader = shader->getVertexShader();
  return vertexShader;
}

// creates the input layout for the renderer/vertex shader
void Shaders::ShaderManager::setup()
{
  ID3DBlob* blob = get("VertexShader")->getBlob();
  ID3D11InputLayout* inputLayout;

  // Define the input layout
  D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
  {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  HRESULT hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);
  if (FAILED(hr))
  {
    throw std::exception("Failed to setup input layout for vertex shader");
  }

  context->IASetInputLayout(inputLayout);
}

ID3D11PixelShader* Shaders::ShaderManager::getPixelShader(std::string name)
{
  return get(name)->getPixelShader();
}
