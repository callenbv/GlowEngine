/*
/
// filename: ModelLibrary.h
// author: Callen Betts
// brief: defines ModelLibrary class
//
// description: Model library stores all of our model data (vertices, indices) so we can simply query the library
//  for a model's data to be rendered instead of loading a new one
/
*/

#pragma once

namespace Models
{

  class ModelLibrary
  {

  public:

    ModelLibrary();

    // load all of our 3D models
    void load(std::string directoryPath = "Assets/Models");

    // add a model to the map
    void add(std::string name, Models::Model* model);

    // get a model from the map
    Models::Model* get(std::string name);

    std::map<std::string, Models::Model*> models; // our models

  };

}