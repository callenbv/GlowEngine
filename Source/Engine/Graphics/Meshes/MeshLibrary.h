/*
/
// filename: MeshLibrary.h
// author: Callen Betts
// brief: defines MeshLibrary class
/
*/

#pragma once

namespace Meshes
{

  class Mesh; // forward declare

  class MeshLibrary
  {

  public:

    // add a new mesh to the library
    void add(Meshes::Mesh* mesh);
    // get a mesh
    Meshes::Mesh* get(std::string name);

    // load all of our preset meshes (quad, etc
    void load();
    // box mesh
    void bindBuffers(std::vector<Vertex> vertices);
    // build box vertices
    void buildVertices(std::vector<Vertex>& out);

    // render common meshes
    void drawBox(Components::BoxCollider* box);
    void drawBox(const Vector3D& pos, const Vector3D& scale, const DirectX::XMFLOAT4& rotQuat);

  private:

      // buffers we can reuse for draw calls
      ID3D11Buffer* indexBuffer;
      ID3D11Buffer* vertexBuffer;

    std::map<std::string, Meshes::Mesh*> meshes;
  };

}