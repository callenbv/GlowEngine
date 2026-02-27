/*
/
// filename: Model.cpp
// author: Callen Betts
// brief: implements Model.h
/
*/

#include "stdafx.h"
#include "Model.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Systems/Parsing/ObjectLoader.h"
#include "Engine/Entity/Components/Visual/Models/ModelLibrary.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include "Engine/Graphics/Meshes/MeshLibrary.h"
#include "Engine/Graphics/Meshes/Mesh.h"

// construct a new base model
Models::Model::Model()
  :
  modelType(ModelType::Cube)
{
  init();
}

// construct a model given a filename
Models::Model::Model(const std::string fileName)
  :
  modelType(ModelType::Cube)
{
  init();
  load(fileName);
}

// initialize the model
void Models::Model::init()
{
  engine = EngineInstance::getEngine();
  renderer = engine->getRenderer(); // models need the renderer to draw
  device = renderer->getDevice();
  deviceContext = renderer->getDeviceContext();
  dirty = true;
  shadowBuffer = nullptr;
  objects = 0;
}

// parse a .obj file and load its vertex data into the model
// adds the model to the model library if it doesn't already exist
void Models::Model::load(const std::string fileName)
{
  // query the model library to see if we can get the model's data
  Models::ModelLibrary* library = engine->getModelLibrary();
  Models::Model* model = library->get(fileName);

  if (model)
  {
    // if the model is in the library, just swap pointers
    // this is only possible because the model data (vertices, indices) isn't ever dynamically changed
    // if we ever need a new unique model, we will need to perform a deep copy
    *this = *model;
  }
  else // if it is not in the library
  {
    // parse a 3D model
    Parse::ObjectLoader modelData;
    // parse model data
    modelData.open(fileName);
    modelData.parseAssimp();

    // store the indices and vertices given the map of object data
    modelVertices = modelData.getModelVertices();
    modelIndices = modelData.getModelIndices();
    modelNames = modelData.getModelNames();
    objects = modelData.getObjects();
    textureNames = modelData.getTextureModelNames();

    // update the buffers for index and vertex data
    // this will also create them if they don't exist
    updateVertexBuffer();
    updateIndexBuffer();
  }

}

// get vertices contianer
const std::vector<GlowMath::Vertex>& Models::Model::getVerticies()
{
  return vertices;
}

const std::vector<GlowMath::Vertex>& Models::Model::getVerticies() const
{
  return vertices;
}

// get indices container
const std::vector<unsigned short>& Models::Model::getIndices()
{
  return indices;
}

ID3D11Buffer* Models::Model::getVertexBuffer(int index)
{
  return vertexBuffers[getModelNames()[index]];
}

ID3D11Buffer* Models::Model::getIndexBuffer(int index)
{
  return indexBuffers[getModelNames()[index]];
}

void Models::Model::renderShadow()
{

}

// creates the vertex buffer or updates it 
void Models::Model::updateVertexBuffer()
{
  // update the model buffers
  for (auto name : modelNames)
  {
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = (UINT)((sizeof(Vertex)) * modelVertices[name].size());
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = modelVertices[name].data();
    renderer->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffers[name]);
  }
  // update the shadow buffer
  Meshes::Mesh* shadowMesh = engine->getMeshLibrary()->get("Quad");

  D3D11_BUFFER_DESC shadowBufferDesc = {};
  shadowBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  shadowBufferDesc.ByteWidth = (UINT)((sizeof(Vertex)) * shadowMesh->getVertices().size());
  shadowBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  shadowBufferDesc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA vertexData = {};
  vertexData.pSysMem = shadowMesh->getVertices().data();
  renderer->getDevice()->CreateBuffer(&shadowBufferDesc, &vertexData, &shadowBuffer);
}

// creates a model's index buffer or updates it 
void Models::Model::updateIndexBuffer()
{
  for (auto name : modelNames)
  {
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = (UINT)((sizeof(unsigned short)) * modelIndices[name].size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = modelIndices[name].data();
    renderer->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffers[name]);
  }
}

// set the texture coordinates to scale with a vector for tiling
void Models::Model::setUV(Vector3D coords)
{
  renderer->SetUVScale(coords.x, coords.y);
}

// set a model's color
void Models::Model::setColor(const float (&col)[4])
{
  // loop through the vertices and set color
  for (auto name : modelNames)
  {
    for (auto& vertex : modelVertices[name])
    {
      vertex.r = col[0];
      vertex.g = col[1];
      vertex.b = col[2];
      vertex.a = col[3];
    }
  }

  // update the vertex buffer
  updateVertexBuffer();
}

// render a model
void Models::Model::render()
{
  // set the index and vertex buffers
  deviceContext->IASetVertexBuffers(0, 1, &vertexBuffers[getModelNames()[objectIndex]], &stride, &offset);
  deviceContext->IASetIndexBuffer(indexBuffers[getModelNames()[objectIndex]], DXGI_FORMAT_R16_UINT, 0);

  // draw the triangle
  deviceContext->DrawIndexed((UINT)modelIndices[getModelNames()[objectIndex]].size(), 0, 0);

  // unbind the texture
  renderer->unBindTexture();
}
