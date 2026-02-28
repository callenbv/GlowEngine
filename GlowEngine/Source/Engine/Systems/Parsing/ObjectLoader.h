/*
/
// filename: ObjectLoader.h
// author: Callen Betts
// brief: defines ObjectLoader class
//
// description: The goal here is to create an instance of the object loader and then parse it
//  this way we can just use methods to return the data parsed so we never have to directly look at it
/
*/

#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Parse
{

  class ObjectLoader
  {

  public:

    ObjectLoader();

    bool open(const std::string fileName);

    void parse(); // parse data and save to indices/vertices
    void parseAssimp(Models::Model* model); // parse model and animation data
    void parseMTL(aiMaterial** materials, Models::Model* modelToLoadInto); // parse MTL data (texture names)
    void processAnimation(const aiScene* scene);

    void close();
    void destroy();

    // get the indices
    const std::vector<Index>& getIndices();
    // get the vertex indices
    const std::vector<unsigned short>& getVertexIndices();
    // get the vertices
    const std::vector<Vertex>& getVertices();

    // return the maps of the data we've stored - this is so we can just query directly
    // get the model vertex map
    std::map<std::string, std::vector<Vertex>> getModelVertices() { return modelVertices; }
    // model indices
    std::map<std::string, std::vector<unsigned short>> getModelIndices() { return modelIndices; }
    // get the model names - this is so we can iterate over each name and get them from the map
    std::vector<std::string> getModelNames() { return modelNames; }
    // get the texture names
    std::vector<std::string> getTextureModelNames() { return textureNames; }
    // get the number of objects
    int getObjects() { return objects; }

    // get the filename
    const std::string getFileName();

  private:

    std::string fileName;
    std::ifstream file;

    // define the vertices and indices containers
    // we have a map of the different vertices and indices for each object
    std::map<std::string, std::vector<Vertex>> modelVertices;
    std::map<std::string, std::vector<unsigned short>> modelIndices;
    // we want to store the model names and texture names so we can organize them and apply them
    std::vector<std::string> modelNames;
    std::vector<std::string> textureNames;

    int objects; // this is nice for keeping track of how many objects

    std::vector<Vertex> vertices;
    std::vector<unsigned short> vertexIndices; // this is vertex indices only

    std::vector<Vector3D> vertexNormals; // unused
    std::vector<Index> indices; // unused
    
  };

}