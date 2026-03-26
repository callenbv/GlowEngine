/*
/
// filename: ShadowSystem.cpp
// author: Callen Betts
// brief: implements ShadowSystem.h
/
*/

#include "stdafx.h"
#include "ShadowSystem.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include "Engine/Graphics/Camera/Camera.h"
#include "Engine/Graphics/Renderer.h"

using namespace DirectX;

Lighting::ShadowSystem::ShadowSystem(ID3D11Device* device_, ID3D11DeviceContext* deviceContext) : System("Shadows"),
	context(deviceContext),
    device(device_),
    buffer(nullptr),
    shadowIndexCount(12),
    indexBuffer(nullptr),
    transformBuffer(nullptr),
    vertexBuffer(nullptr)
{

}


struct VertexTwo 
{
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT2 texcoord;
};

void Lighting::ShadowSystem::DrawShadow(const Vector3D& position, Vector3D scale)
{
  Graphics::Renderer* renderer = engine->getRenderer();

  // Define vertices and indices for a quad
  std::vector<Vertex> vertices = {
      { -1.0f,  1.0f, 0.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f },
      {  1.0f,  1.0f, 0.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f },
      {  1.0f, -1.0f, 0.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f },
      { -1.0f, -1.0f, 0.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f }
  };
  int indices[] = {
      0, 1, 2,
      0, 2, 3
  };

  // Define vertices
  D3D11_BUFFER_DESC vertexBufferDesc;
  ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * vertices.size());
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA vertexData;
  vertexData.pSysMem = vertices.data();
  HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

  D3D11_BUFFER_DESC indexBufferDesc;
  ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(indices);
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
   
  D3D11_SUBRESOURCE_DATA indexData;
  ZeroMemory(&indexData, sizeof(indexData));
  indexData.pSysMem = indices;

  result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

  DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, -9.99f, position.z);
  DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
  DirectX::XMFLOAT4 rotation = { 0,0,0,0 };
  DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationRollPitchYaw(angleInRadians, 0.0f, 0.0f);
  DirectX::XMStoreFloat4(&rotation, quaternion);

  // Create the rotation matrix from the quaternion
  DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));

  DirectX::XMMATRIX final = scaleMatrix * rotationMatrix * translation;

  ID3D11ShaderResourceView* texture = *EngineInstance::getEngine()->getTextureLibrary()->get("Shadow")->getTextureView();
  context->PSSetShaderResources(0, 1, &texture);

  // update the constant buffer's world matrix
  renderer->updateObjectBufferWorldMatrix(final);
  renderer->updateObjectBuffer();

  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
  context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

  context->DrawIndexed(6, 0, 0);

  indexBuffer->Release();
  vertexBuffer->Release();
}
