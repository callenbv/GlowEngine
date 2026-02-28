/*
/
// filename: MeshLibrary.h
// author: Callen Betts
// brief: defines MeshLibrary class
/
*/

#pragma once

namespace Materials
{

  class Material; // forward declare

  class MaterialLibrary
  {

  public:

    // add a new material to the library
    void add(Materials::Material* mesh);
    // get a mesh
    Materials::Material* get(std::string name);

    // load all of our preset material
    void load();

  private:

    std::map<std::string, Materials::Material*> materials;

  };

}