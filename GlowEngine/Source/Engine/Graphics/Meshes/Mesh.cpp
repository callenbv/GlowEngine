/*
/
// filename: Mesh.cpp
// author: Callen Betts
// brief: implements Mesh.h
/
*/

#include "stdafx.h"
#include "Mesh.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Meshes/MeshLibrary.h"
#include "Engine/Graphics/Renderer.h"

// create a mesh
Meshes::Mesh::Mesh(std::string meshName)
{
  init();
  name = meshName;
  library->add(this);
}

// initialize a mesh's data
void Meshes::Mesh::init()
{
  library = EngineInstance::getEngine()->getMeshLibrary();
  stride = sizeof(Vertex);
  offset = 0;
}

// set a mesh's vertices
void Meshes::Mesh::setVertices(const std::vector<Vertex>& vertexList)
{
  for (auto& v : vertexList)
  {
    vertices.push_back(v);
  }
}

// set a mesh's indices
void Meshes::Mesh::setIndices(std::vector<unsigned short> indexList)
{
  for (auto i : indexList)
  {
    indices.push_back(i);
  }
}

void Meshes::Mesh::render()
{
    // update the buffers if they are not made yet
    if (!vertexBuffer)
    {
        updateVertexBuffer();
    }
    if (!indexBuffer)
    {
        updateIndexBuffer();
    }

    // set the index and vertex buffers
    EngineInstance::getEngine()->getRenderer()->getDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    EngineInstance::getEngine()->getRenderer()->getDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // draw the triangle
    EngineInstance::getEngine()->getRenderer()->getDeviceContext()->DrawIndexed(indices.size(), 0, 0);
}

// creates the vertex buffer or updates it 
void Meshes::Mesh::updateVertexBuffer()
{
    // update the model buffers
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = (UINT)((sizeof(Vertex)) * vertices.size());
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();
    EngineInstance::getEngine()->getRenderer()->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
}

void Meshes::Mesh::addVertex(Vertex vertex)
{
    vertices.push_back(vertex);
}

void Meshes::Mesh::addIndex(unsigned short index)
{
    indices.push_back(index);
}

// creates a model's index buffer or updates it 
void Meshes::Mesh::updateIndexBuffer()
{
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = (UINT)((sizeof(unsigned short)) * indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();
    EngineInstance::getEngine()->getRenderer()->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}
