/*
/
// filename: Model.h
// author: Callen Betts
// brief: defines the model class, holds vertices and model data
/
*/

/* 

HOW IT WORKS:

// we have a map of the different vertices and indices for each object
// this is because a model might be exported with several objects "attached"
// for example, if we have two cubes in blender and we want to export them, it will only render the original cube
//  or the first cube it read. By having a map of the different objects that were exported, we can individually save
//  and keep track of each one's vertices and indices. The map will store the object's name (for example, "cube1" and "cube2")
//  and each of that object's respective data.
//
// modelVertices refers to the actual map of all of the object's vertices 
// modelIndices refers to the map of all the object's indices, rather than individual objects

*/

#pragma once
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "Engine/Graphics/Meshes/Mesh.h"

// forward declare renderer
namespace Graphics
{
  class Renderer;
}

namespace Models
{

  class Model
  {

    // enum for primitive 3D models (cubes, spheres, etc.)
    enum ModelType
    {
      Cube,
      Sphere,
      Pyramid
    };

  public:

    Model();
    Model(const std::string fileName);

    void init();
    void load(const std::string fileName);
    void addMesh(Meshes::Mesh* meshToAdd);
    std::vector<Meshes::Mesh*> getMeshes();

    // is dirty
    bool isDirty() { return dirty; }

    // render a model
    void render();
    // rename a model
    void setName(std::string name);
    std::string& getName();

    std::vector<Vertex> allVertices; // all vertices in the model, cached once when loading

  private:
    
    bool dirty; // recalculate vertices, indices, or texture coords

    // give the models access to the renderer/engine
    Engine::GlowEngine* engine;
    Graphics::Renderer* renderer;

    // model contains a list of meshes
    std::vector<Meshes::Mesh*> meshes;
    std::string name;

    // all inherited objects have these properties
    ModelType modelType;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
  };
}