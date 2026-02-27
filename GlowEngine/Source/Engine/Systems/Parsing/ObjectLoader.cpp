/*
/
// filename: ObjectLoader.cpp
// author: Callen Betts
// brief: implements ObjectLoader.h
/
*/

#include "stdafx.h"
#include "ObjectLoader.h"
#include <sstream>

// default constructor for object loading
Parse::ObjectLoader::ObjectLoader()
  :
  fileName("none"),
  objects(0)
{
}

// replace a filename's extension
static std::string replaceFileExtension(const std::string& filename, const std::string& newExtension) 
{
  std::string newFilename = filename;
  size_t pos = newFilename.rfind(".obj");
  if (pos != std::string::npos) { // Check if the .obj extension is found
    newFilename.replace(pos, 4, newExtension); // Replace .obj with new extension
  }
  return newFilename;
}

// attempt to open a file - this saves file path and file data
bool Parse::ObjectLoader::open(const std::string filePath)
{
  fileName = filePath;
  file = std::ifstream(fileName);

  bool opened = file.is_open();
  
  if (!opened)
  {
    // error opening file
    Logger::write("Failed to open ", fileName);
  }

  return opened;
}

// parse a the file's contents
// this will iterate over the .obj file and save vertices, indices, and other core data
void Parse::ObjectLoader::parse()
{
  std::vector<Vector3D> temp_normals;
  std::vector<Vector3D> temp_uvs;

  std::string currentObjectName = "default"; // this will store the name of each object

  // loop through each line within the file and check for a prefix of data
  std::string line;
  while (std::getline(file, line)) 
  {
    // get the data prefix so we know type of data to read
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;

    // get the name of each object 
    if (prefix == "o" || prefix == "g") 
    {
      // read the new object name and use it for subsequent data
      iss >> currentObjectName;
      modelNames.push_back(currentObjectName);
      objects++;
    }
    else if (prefix == "v") 
    {
      // load each vertex
      Vertex vertex{ 0,0,0,1,1,1,1 };
      iss >> vertex.x >> vertex.y >> vertex.z;

      modelVertices[currentObjectName].push_back(vertex); // add the vertex to object map
      vertices.push_back(vertex);
    }
    else if (prefix == "vt")
    {
      // load texture coordinates
      Vector3D uv;
      iss >> uv.x >> uv.y;
      temp_uvs.push_back(uv);
    }
    else if (prefix == "vn")
    {
      // vertex normals
      Vector3D normal;
      iss >> normal.x >> normal.y >> normal.z;
      temp_normals.push_back(normal);
    }
    else if (prefix == "f") 
    {
      // treat each new object
      std::string vertexData;
      while (iss >> vertexData) 
      {
        std::istringstream vertexStream(vertexData);
        int posIndex, texIndex = -1, normIndex = -1;
        char slash;

        vertexStream >> posIndex; // Read the position index
        if (vertexStream >> slash) vertexStream >> texIndex; // Optionally read texture coordinate index
        if (vertexStream >> slash) vertexStream >> normIndex; // Optionally read normal index

        // Adjust for OBJ's 1-based indexing
        posIndex--;
        if (texIndex > 0) texIndex--;
        if (normIndex > 0) normIndex--;

        // Construct a complete vertex with position, normal, and texCoord if available
        Vertex completeVertex = vertices[posIndex]; // Use local vertex data
        if (texIndex >= 0 && texIndex < temp_uvs.size()) 
        {
          Vector3D uv = temp_uvs[texIndex];
          completeVertex.tx = uv.x;
          completeVertex.ty = uv.y;
        }
        if (normIndex >= 0 && normIndex < temp_normals.size()) 
        {
          Vector3D normal = temp_normals[normIndex];
          completeVertex.nx = normal.x;
          completeVertex.ny = normal.y;
          completeVertex.nz = normal.z;
        }

        // Add the complete vertex to the global model vertices for the current object
        modelVertices[currentObjectName].push_back(completeVertex);

        // Add the index of the complete vertex to the global model indices for the current object
        modelIndices[currentObjectName].push_back(static_cast<short>(modelVertices[currentObjectName].size() - 1));
      }
    }
  }

  // parse the MTL parts of the file
  parseMTL();

  // close the file
  close();
}

// parse a model using assimp loader
void Parse::ObjectLoader::parseAssimp()
{
  Assimp::Importer importer;

  // Read the OBJ file, and apply post-processing steps for triangulation and generating normals
  const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenNormals);

  // Check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
  {
    Logger::error("Failed to load model " + fileName);
    return;
  }

  // Process all the scene's meshes
  for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    const aiMesh* mesh = scene->mMeshes[i];
    
    // get the mesh name
    std::string meshName = mesh->mName.C_Str();
    modelNames.push_back(meshName);
    objects++;

    // Process vertices
    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
      
      // create the vertex
      Vertex vertex;

      // position
      aiVector3D pos = mesh->mVertices[j];
      vertex.x = pos.x;
      vertex.y = pos.y;
      vertex.z = pos.z;

      // normals
      if (mesh->HasNormals()) {
        aiVector3D normal = mesh->mNormals[j];
        vertex.nx = normal.x;
        vertex.ny = normal.y;
        vertex.nz = normal.z;
      }

      // Texture coordinates
      if (mesh->mTextureCoords[0]) {
        aiVector3D uv = mesh->mTextureCoords[0][j];
        vertex.tx = uv.x;
        vertex.ty = uv.y;
      }

      // base color
      vertex.r = 1;
      vertex.g = 1;
      vertex.b = 1;
      vertex.a = 1;

      // add the vertex
      modelVertices[meshName].push_back(vertex);
    }

    for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
      aiFace face = mesh->mFaces[j];
      // loop through the actual indices within each face
      for (unsigned int k = 0; k < face.mNumIndices; k++) {
        // add the indices to the model
        unsigned short index = face.mIndices[k];
        modelIndices[meshName].push_back(index);
      }
    }

    // Process material properties if necessary
    if (mesh->mMaterialIndex >= 0) {
      // currently we have no materials
    }
  }

  // parse the mtl parts of the file which contain texture names
  parseMTL();

  // TO-DO: Parse FBX animation data
  processAnimation(scene);

  // close the file
  close();
}

// parse MTL data - this contains things like texture names
void Parse::ObjectLoader::parseMTL()
{
  std::string filePath = replaceFileExtension(fileName, ".mtl");
  std::ifstream mtl(filePath);

  bool open = mtl.is_open();
  // open the mtl file
  if (open)
  {
    std::string line;
    std::string currentMaterialName;

    // read in each line
    while (std::getline(mtl, line))
    {
      // open the file
      std::istringstream lineStream(line);
      std::string prefix;
      lineStream >> prefix;

      // read in the material name - this is currently unused
      if (prefix == "newmtl")
      {
        lineStream >> currentMaterialName;
      }
      else if (prefix == "map_Kd")
      {
        // get the texture file path
        std::string textureFilePath;
        lineStream >> textureFilePath;

        // extract the file name from the full file path
        size_t lastSlash = textureFilePath.find_last_of("/\\"); // Handles both forward and backslashes
        std::string textureFileName = textureFilePath.substr(lastSlash + 1);

        // remove the extension from the texture file name
        size_t lastDot = textureFileName.rfind('.');
        if (lastDot != std::string::npos)
        {
          textureFileName.erase(lastDot);
        }

        // save the texture file name without the path and extension
        textureNames.push_back(textureFileName);
      }
    }
  }
  else
  {
    // failed to open
    return;
  }
}

// process the animation data from the scene
void Parse::ObjectLoader::processAnimation(const aiScene* scene)
{
  if (scene->HasAnimations()) {
    for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
      const aiAnimation* anim = scene->mAnimations[i];
      for (unsigned int j = 0; j < anim->mNumChannels; j++) {
        const aiNodeAnim* channel = anim->mChannels[j];
       
        for (unsigned int i = 0; i < anim->mNumChannels; i++) {
          const aiNodeAnim* channel = anim->mChannels[i];
          std::string nodeName = channel->mNodeName.C_Str();

          // Process position keyframes
          for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
            aiVectorKey key = channel->mPositionKeys[j];
            float time = static_cast<float>(key.mTime);
            aiVector3D position = key.mValue;
            // Store the position and time in your animation data structure
          }

          // Process rotation keyframes
          for (unsigned int j = 0; j < channel->mNumRotationKeys; j++) {
            aiQuatKey key = channel->mRotationKeys[j];
            float time = static_cast<float>(key.mTime);
            aiQuaternion rotation = key.mValue;
            // Store the rotation and time in your animation data structure
          }

          // Process scaling keyframes
          for (unsigned int j = 0; j < channel->mNumScalingKeys; j++) {
            aiVectorKey key = channel->mScalingKeys[j];
            float time = static_cast<float>(key.mTime);
            aiVector3D scale = key.mValue;
            // Store the scale and time in your animation data structure
          }
        }
      }
    }
  }
}


// delete the parser and its data from memory once we're finished
void Parse::ObjectLoader::close()
{
  if (file.is_open())
  {
    file.close();
  }
}

// kill ourselves
void Parse::ObjectLoader::destroy()
{
  delete this;
}

// get the index container
const std::vector<Index>& Parse::ObjectLoader::getIndices()
{
  return indices;
}

// get the vertex index container
const std::vector<unsigned short>& Parse::ObjectLoader::getVertexIndices()
{
  return vertexIndices;
}

// get the vertex container
const std::vector<Vertex>& Parse::ObjectLoader::getVertices()
{
  return vertices;
}

// get the file path of the object file
const std::string Parse::ObjectLoader::getFileName()
{
  return fileName;
}

