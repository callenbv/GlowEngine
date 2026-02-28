/*
/
// filename: Mesh.h
// author: Callen Betts
// brief: defines Mesh class
/
*/

#pragma once

namespace Graphics
{
    class Renderer;
}

namespace Meshes
{
  class MeshLibrary; // forward declare

  // mesh subsections define which index ranges we apply materials to
  struct MeshSubSection
  {
      unsigned short first;
      unsigned short last;

      std::string materialName;
      //Materials::Material* materialIndex = nullptr; // TODO: cached reference to material index
  };

  // main mesh class
  class Mesh
  {

  public:

    // construct a new mesh
    Mesh(std::string name = "Quad");

    // initialize a new mesh
    void init();

    // set the mesh's vertices
    void setVertices(const std::vector<Vertex>& vertexList);
    // set the mesh's indices
    void setIndices(std::vector<unsigned short> indexList);

    // get the mesh's data for read only
    const std::vector<Vertex>& getVertices() { return vertices; }
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<unsigned short>& getIndices() { return indices; }

    // update the mesh's buffers
    void updateIndexBuffer();
    void updateVertexBuffer();

    // add or remove indices/vertices
    void addVertex(Vertex vertex);
    void addIndex(unsigned short index);

    // render the mesh
    void render();

    // get the name
    std::string getName() { return name; }
    std::vector<MeshSubSection>&getMeshSubsections() { return sections; }
    void addSection(MeshSubSection section) { sections.push_back(section); }

  private:

    Meshes::MeshLibrary* library;

    std::string name;

    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;

    // the ranges to map materials to indices
    std::vector<MeshSubSection> sections;

    // vertex buffers
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    UINT offset;
    UINT stride;
  };
}