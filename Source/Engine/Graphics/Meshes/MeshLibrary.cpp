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
#include "Engine/Graphics/Materials/Material.h"
#include "Engine/Graphics/Materials/MaterialLibrary.h"
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

// setup the buffers for indices and vertices
void Meshes::MeshLibrary::bindBuffers(std::vector<Vertex> vertices)
{
    ID3D11Device* device = EngineInstance::getEngine()->getRenderer()->getDevice();

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
}

void Meshes::MeshLibrary::buildVertices(std::vector<Vertex>& out)
{
    out.clear();
    out.reserve(24);

    const float x0 = -0.5f, x1 = 0.5f;
    const float y0 = -0.5f, y1 = 0.5f;
    const float z0 = -0.5f, z1 = 0.5f;

    auto V = [](float x, float y, float z, float nx, float ny, float nz, float u, float v) -> Vertex
        {
            Vertex vert{};
            vert.x = x;
            vert.y = y;
            vert.z = z;
            vert.nx = nx;
            vert.ny = ny;
            vert.nz = nz;

            vert.tx = u;
            vert.ty = v;

            return vert;
        };

    // +Z (front)
    out.push_back(V(x0, y0, z1, 0, 0, 1, 0, 1));
    out.push_back(V(x0, y1, z1, 0, 0, 1, 0, 0));
    out.push_back(V(x1, y1, z1, 0, 0, 1, 1, 0));
    out.push_back(V(x1, y0, z1, 0, 0, 1, 1, 1));

    // -Z (back)
    out.push_back(V(x1, y0, z0, 0, 0, -1, 0, 1));
    out.push_back(V(x1, y1, z0, 0, 0, -1, 0, 0));
    out.push_back(V(x0, y1, z0, 0, 0, -1, 1, 0));
    out.push_back(V(x0, y0, z0, 0, 0, -1, 1, 1));

    // +X (right)
    out.push_back(V(x1, y0, z1, 1, 0, 0, 0, 1));
    out.push_back(V(x1, y1, z1, 1, 0, 0, 0, 0));
    out.push_back(V(x1, y1, z0, 1, 0, 0, 1, 0));
    out.push_back(V(x1, y0, z0, 1, 0, 0, 1, 1));

    // -X (left)
    out.push_back(V(x0, y0, z0, -1, 0, 0, 0, 1));
    out.push_back(V(x0, y1, z0, -1, 0, 0, 0, 0));
    out.push_back(V(x0, y1, z1, -1, 0, 0, 1, 0));
    out.push_back(V(x0, y0, z1, -1, 0, 0, 1, 1));

    // +Y (top)
    out.push_back(V(x0, y1, z1, 0, 1, 0, 0, 1));
    out.push_back(V(x0, y1, z0, 0, 1, 0, 0, 0));
    out.push_back(V(x1, y1, z0, 0, 1, 0, 1, 0));
    out.push_back(V(x1, y1, z1, 0, 1, 0, 1, 1));

    // -Y (bottom)
    out.push_back(V(x0, y0, z0, 0, -1, 0, 0, 1));
    out.push_back(V(x0, y0, z1, 0, -1, 0, 0, 0));
    out.push_back(V(x1, y0, z1, 0, -1, 0, 1, 0));
    out.push_back(V(x1, y0, z0, 0, -1, 0, 1, 1));
}

// draw a cube mesh
// Overload that draws a unit cube mesh using pos/scale/rotation
void Meshes::MeshLibrary::drawBox(const Vector3D& pos, const Vector3D& scale, const DirectX::XMFLOAT4& rotQuat)
{
    Graphics::Renderer* renderer = EngineInstance::getEngine()->getRenderer();
    auto* ctx = renderer->getDeviceContext();

    std::vector<Vertex> vertices;

    // Build world matrix
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotQuat));
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

    DirectX::XMMATRIX world = S * R * T;

    // Set unlit shader
    renderer->setPixelShader("UnlitPixelShader");

    // Add vertices to the vector
    buildVertices(vertices);
    bindBuffers(vertices);

    renderer->updateObjectBufferWorldMatrix(world);
    renderer->updateObjectBuffer();
    renderer->BindMaterial(EngineInstance::getEngine()->getMaterialLibrary()->get("RedMaterial"));

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    ctx->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    ctx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    ctx->DrawIndexed(36, 0, 0);

    renderer->setPixelShader("PixelShader");
}

// Your existing collider convenience wrapper can just forward:
void Meshes::MeshLibrary::drawBox(Components::BoxCollider* box)
{
    Vector3D scale = box->getHitboxSize();
    Vector3D pos = getComponentOfType(Transform, box->parent)->getPosition();

    // Identity rotation unless you have a real quaternion to pass in
    DirectX::XMFLOAT4 rot = { 0, 0, 0, 1 };

    drawBox(pos, scale, rot);
}
