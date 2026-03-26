/*
/
// filename: ObjectLoader.cpp
// author: Callen Betts
// brief: implements ObjectLoader.h
/
*/

#include "stdafx.h"
#include "ObjectLoader.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Materials/Material.h"
#include "Engine/Graphics/Materials/MaterialLibrary.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include <sstream>

static inline void ltrim(std::string& s) 
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
}

static inline void rtrim(std::string& s) 
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
}

static inline void trim(std::string& s) { ltrim(s); rtrim(s); }

static inline std::string stripQuotes(std::string s) 
{
    trim(s);
    if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\'')))
        return s.substr(1, s.size() - 2);
    return s;
}

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

// parse a the file's contents (custom OBJ loader)
// this will iterate over the .obj file and build the meshes
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
  parseMTL(nullptr,nullptr);

  // close the file
  close();
}

// parse a model using assimp loader
void Parse::ObjectLoader::parseAssimp(Models::Model* modelToLoadInto)
{
    if (!modelToLoadInto)
        return;

    Assimp::Importer importer;

    // Read the OBJ file, and apply post-processing steps for triangulation and generating normals
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenNormals);

    // Check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        Logger::error("Failed to load model " + fileName);
        return;
    }

    // parse the mtl parts of the file which contain texture names
    parseMTL(scene,modelToLoadInto);

    // Process all the scene's meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[i];
    
        // get the mesh name
        std::string meshName = mesh->mName.C_Str();
        modelNames.push_back(meshName);
        objects++;

        // create the new mesh
        Meshes::Mesh* gMesh = new Meshes::Mesh(meshName);

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
            gMesh->addVertex(vertex);
            modelToLoadInto->allVertices.push_back(vertex);
        }

        // add the indices
        unsigned short indexNum = 0;

        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            // loop through the actual indices within each face
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                // add the indices to the model
                unsigned short index = face.mIndices[k];
                gMesh->addIndex(index);
                indexNum++;
            }
        }

        // parse the material subsections using index data
        Meshes::MeshSubSection section;
        section.first = 0;
        section.last = indexNum;
        section.materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
        gMesh->addSection(section);
        auto* aMat = scene->mMaterials[mesh->mMaterialIndex];

        // add the mesh to the model
        modelToLoadInto->addMesh(gMesh);
    }
}

// parse MTL data - this contains things like texture names
void Parse::ObjectLoader::parseMTL(const aiScene* scene, Models::Model* modelToLoadInto)
{
    if (!modelToLoadInto || !scene)
        return;

    std::string filePath = replaceFileExtension(fileName, ".mtl");
    std::ifstream mtl(filePath);

    bool open = mtl.is_open();

    // open the mtl file
    if (open)
    {
        for (unsigned i = 0; i < scene->mNumMaterials; ++i)
        {
            aiMaterial* sceneMat = scene->mMaterials[i];

            // get the name of the material
            std::string currentMaterialName = sceneMat->GetName().C_Str();

            // assign material information before adding to the library
            // first check if this material exists in the library before creating it
            if (EngineInstance::getEngine()->getMaterialLibrary()->get(currentMaterialName) == nullptr)
            {
                Materials::Material* material = new Materials::Material();

                sceneMat->Get(AI_MATKEY_SHININESS, material->shininess);
                sceneMat->Get(AI_MATKEY_OPACITY, material->dissolve);

                aiColor3D kd(1, 1, 1), ka(0, 0, 0), ks(0, 0, 0);
                sceneMat->Get(AI_MATKEY_COLOR_DIFFUSE, kd);
                sceneMat->Get(AI_MATKEY_COLOR_AMBIENT, ka);
                sceneMat->Get(AI_MATKEY_COLOR_SPECULAR, ks);

                material->diffuseColor = { kd.r, kd.g, kd.b };
                material->ambientColor = { ka.r, ka.g, ka.b };
                material->specularColor = { ks.r, ks.g, ks.b };

                material->setName(currentMaterialName);

                aiString diffuseTexturePath;
                aiReturn success = sceneMat->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexturePath);

                if (success == AI_SUCCESS)
                {
                    std::filesystem::path p(diffuseTexturePath.C_Str());
                    std::string fName = p.stem().string();
                    material->diffuseTexture = EngineInstance::getEngine()->getTextureLibrary()->get(fName);
                }

                // add to the material library
                EngineInstance::getEngine()->getMaterialLibrary()->add(material);
                Logger::write("Added new material " + currentMaterialName);
            }
            else
            {
                Logger::write("Used cached material " + currentMaterialName);
            }
        }
    }
}

// process MTL files and load it into a material
void Parse::ObjectLoader::parseMTLIsolated(Materials::Material* mat)
{
    if (!mat) return;

    std::ifstream mtl(fileName);
    if (!mtl.is_open())
    {
        Logger::write("Failed to open MTL: " + fileName);
        return;
    }

    // directory of the .mtl so relative texture paths resolve.
    const std::filesystem::path mtlPath(fileName);
    const std::filesystem::path mtlDir = mtlPath.has_parent_path() ? mtlPath.parent_path() : std::filesystem::path{};

    bool appliedFirstNewMtl = false;

    std::string line;
    while (std::getline(mtl, line))
    {
        // remove comments
        if (auto hash = line.find('#'); hash != std::string::npos)
            line = line.substr(0, hash);

        trim(line);
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string key;
        iss >> key;

        // handle multiple spaces and tabs
        if (key == "newmtl")
        {
            std::string name;
            std::getline(iss, name);
            name = stripQuotes(name);
            trim(name);

            if (!appliedFirstNewMtl && !name.empty())
            {
                mat->setName(name);
                appliedFirstNewMtl = true;
            }
        }
        else if (key == "Kd")
        {
            float r, g, b;
            if (iss >> r >> g >> b)
                mat->diffuseColor = { r, g, b };
        }
        else if (key == "Ka")
        {
            float r, g, b;
            if (iss >> r >> g >> b)
                mat->ambientColor = { r, g, b };
        }
        else if (key == "Ks")
        {
            float r, g, b;
            if (iss >> r >> g >> b)
                mat->specularColor = { r, g, b };
        }
        else if (key == "Ns")
        {
            float ns;
            if (iss >> ns)
                mat->shininess = ns;
        }
        else if (key == "d")
        {
            float d;
            if (iss >> d)
                mat->dissolve = d; // opacity
        }
        else if (key == "Tr")
        {
            float tr;
            if (iss >> tr)
                mat->dissolve = 1.0f - tr; // common convention
        }
        else if (key == "map_Kd")
        {
            // map_Kd can include options like "-s 1 1 1" or "-o 0 0 0"
            // we'll parse tokens and take the last token as the path (good enough for most exporters)
            std::string tok, last;
            while (iss >> tok) last = tok;

            if (!last.empty())
            {
                last = stripQuotes(last);

                // Resolve relative paths against the .mtl directory
                std::filesystem::path texPath = std::filesystem::path(last);
                if (texPath.is_relative())
                    texPath = mtlDir / texPath;

                // You’re currently keying textures by stem name:
                std::string keyName = texPath.stem().string();

                auto* texLib = EngineInstance::getEngine()->getTextureLibrary();
                auto* found = texLib->get(keyName);

                if (found)
                {
                    mat->diffuseTexture = found;
                }
            }
        }
    }

    // If the MTL had no newmtl line, keep existing mat name (or set from filename)
    if (mat->getName().empty())
        mat->setName(mtlPath.stem().string());
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

// get the file path of the object file
const std::string Parse::ObjectLoader::getFileName()
{
  return fileName;
}

