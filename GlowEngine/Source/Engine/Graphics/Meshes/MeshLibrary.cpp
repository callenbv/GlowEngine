/*
/
// filename: MeshLibrary.cpp
// author: Callen Betts
// brief: implements MeshLibrary.h
/
*/

#include "stdafx.h"
#include "MeshLibrary.h"
#include "Engine/Graphics/Meshes/Mesh.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Entity/Components/Visual/Models/ModelLibrary.h"
#include "Engine/GlowEngine.h"

// add a mesh to the library
void Meshes::MeshLibrary::add(Meshes::Mesh* mesh)
{
  if (mesh)
  {
    // add mesh if it was valid
    meshes[mesh->getName()] = mesh;
    Logger::write("Added new mesh "+mesh->getName());
  }
  else
  {
    // invalid mesh
    Logger::error("Failed to add mesh " + mesh->getName() + " to library");
  }
}

// get a mesh given a name
Meshes::Mesh* Meshes::MeshLibrary::get(std::string name)
{
  return meshes[name];
}

// load all of our preset meshes
void Meshes::MeshLibrary::load()
{
  // ** Quad Mesh ** //
  Meshes::Mesh* quadMesh = new Meshes::Mesh("Quad");
  // vertices
  std::vector<Vertex> vertices = 
  {
    // Vertex 1
    { -0.5f, 0, 1,
      1.0f,  1.0f,  1.0f, 1.0f,
      0,1,0,
      1.0f,  0.0f },

    // Vertex 2
    { 1,0,1,
      1.0f,  1.0f,  1.0f, 1.0f,
      0,1,0,
      0.0f,  1.0f },  // Texture coordinates

    // Vertex 3
    { -1,0,-1,
      1.0f,  1.0f,  1.0f, 1.0f,
      0,1,0,
      0.0f,  0.0f },  // Texture coordinates

    // Vertex 4
    { 1,0,-1,
      1.0f,  1.0f,  1.0f, 1.0f,
      0,1,0,
      1.0f,  1.0f }   // Texture coordinates
  };
  quadMesh->setVertices(vertices);
  // indices
  std::vector<unsigned short> indices =
  {
    0,1,2,
    0,2,3
  };
  quadMesh->setIndices(indices);
}

// draw a cube mesh
void Meshes::MeshLibrary::drawBox(Components::BoxCollider* box)
{
  std::vector<Vertex> vertices = box->getVertices();

  if (vertices.empty())
    return;

  Graphics::Renderer* renderer = EngineInstance::getEngine()->getRenderer();
  ID3D11DeviceContext* deviceContext = renderer->getDeviceContext();
  ID3D11Device* device = renderer->getDevice();
  
  ID3D11Buffer* vertexBuffer;
  ID3D11Buffer* indexBuffer;

  // Define vertices
  D3D11_BUFFER_DESC vertexBufferDesc;
  ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * vertices.size());
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA vertexData;
  ZeroMemory(&vertexData, sizeof(vertexData));
  vertexData.pSysMem = vertices.data();

  HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

  std::vector<unsigned> indices = {
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        0, 1, 5, 5, 4, 0, // Bottom face
        2, 3, 7, 7, 6, 2, // Top face
        0, 3, 7, 7, 4, 0, // Left face
        1, 2, 6, 6, 5, 1  // Right face
  };

  D3D11_BUFFER_DESC indexBufferDesc;
  ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = (UINT)(sizeof(unsigned) * indices.size());
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA indexData;
  ZeroMemory(&indexData, sizeof(indexData));
  indexData.pSysMem = indices.data();

  result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

  Vector3D scale = box->getMeshScale();
  Vector3D pos = getComponentOfType(Transform,box->parent)->getPosition();
  DirectX::XMFLOAT4 rotation = { 0,0,0,0 };

  DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
  DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation)); // Assuming rotation is an XMFLOAT4 representing a quaternion
  DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

  // Combine the transformations
  DirectX::XMMATRIX final = scaleMatrix * rotationMatrix * translationMatrix;

  // update the constant buffer's world matrix
  renderer->updateObjectBufferWorldMatrix(final);

  renderer->updateObjectBuffer();

  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
  deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

  deviceContext->DrawIndexed(36, 0, 0);

  indexBuffer->Release();
  vertexBuffer->Release();
}
