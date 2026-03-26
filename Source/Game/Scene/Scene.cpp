/*
/
// filename: Scene.cpp
// author: Callen Betts
// brief: implements Scene.h
/
*/

#include "stdafx.h"
#include "Scene.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/Camera/Camera.h"

// base scene constructor
Scene::Scene::Scene()
{
  engine = EngineInstance::getEngine();
  input = engine->getInputSystem();
  factory = engine->getEntityFactory();
  globalList = new Entities::EntityList();
  rootList = new Entities::EntityList();
  rootList->add(engine->getCamera());
  rootList->SetName("Root");
  name = "Scene";
}

/// <summary>
/// Iterates through every entity in the scene and saves entities to a temporary file
/// This way, we can play the game and revert back to how it was before
/// </summary>
void Scene::Scene::SaveSnapshot(std::string filePath)
{
  // Create a file to save to
  std::ofstream temp(filePath);

  nlohmann::json sceneData;
  nlohmann::json entityListData;

  // Iterate over each of our entities and save their data to a temporary file
  std::vector<Entities::EntityList*> lists = rootList->getSubLists();
  lists.push_back(rootList);

  for (const auto& list : lists)
  {
    entityListData[list->getName()] = list->Save();
  }

  sceneData["Entities"] = entityListData;

  // Save the json file
  temp << sceneData.dump(4);
  temp.close();

  Logger::write("Saved snapshot successfully to " + filePath);
}

/// <summary>
/// Load the scene back to its default state using the temporary snapshot
/// </summary>
void Scene::Scene::LoadSnapshot(std::string filePath)
{
  // Reload entities from temporary file
  std::ifstream file(filePath);

  // If we can't find a temporary file, this is really bad. In the future, if this happens
  // (The temp file was deleted) we want to revert to a previous hard save.
  if (!file.is_open())
  {
    Logger::write("Failed to find temp scene snapshot " + filePath);
    return;
  }

  // Check if the file is empty
  file.seekg(0, std::ios::end);  // Move the file pointer to the end
  if (file.tellg() == 0)  // If the file pointer is at position 0, the file is empty
  {
    Logger::write("Snapshot file is empty " + filePath);
    return;
  }
  file.seekg(0, std::ios::beg);

  // Destroy every single entity in existence (clean slate)
  rootList->DeleteAllEntities();

  // Load everything from the snapshot
  nlohmann::json sceneData;

  file >> sceneData;

  // Iterate over every entity in scene and load it back in
  for (const auto& [entryName, fileData] : sceneData.items())
  {
    // Recreate an entity with the saved data
    if (entryName == "Entities")
    {
      // Iterate over entity lists
      for (const auto& [entityList, entityListData] : fileData.items())
      {
        // Add each entity within the list
        for (const auto& [entity, entityData] : entityListData.items())
        {
          // Construct the entity
          Entities::Entity* newEntity = new Entities::Entity(entity);
          newEntity->Load(entityData);
          
          // Try to find the list associated with it (TODO)
          rootList->add(newEntity);
        }
      }
    }
  }

  Logger::write("Loaded snapshot from " + filePath);
}

/// <summary>
/// Saves every part of a scene, including settings
/// For now, this will just take a snapshot of all current entities
/// </summary>
void Scene::Scene::SaveScene()
{
  SaveSnapshot("Data/Maps/"+name+"_MapData.json");
}

/// <summary>
/// Load a scene's saved data when scene is opened
/// </summary>
void Scene::Scene::LoadScene()
{
  LoadSnapshot("Data/Maps/"+name+"_MapData.json");
}

// exit a given scene; this doesn't do anything right now, because we have no data we want to unload
// this may be useful in the future, but for now it will just restart the scene
void Scene::Scene::exit()
{
  restart();
}

void Scene::Scene::restart()
{
  clear();
  init();
}

// render a scene
void Scene::Scene::render()
{
  renderEntities();
}

// update a scene's given entities
void Scene::Scene::updateEntities()
{
  // update all of our entity lists; entity lists recursively update their sublists
  rootList->update();

  // we have a separate list for handling collisions
  globalList->updateColliders();
}

// render a scene's entities
void Scene::Scene::renderEntities()
{
  rootList->render();
}

// create an entity within a scene that will directly add it to the list
// easy way to create new entities
Entities::Actor* Scene::Scene::createEntity(
  Vector3D pos, 
  Vector3D scale, 
  Vector3D rotation, 
  std::string modelName, 
  std::string textureName)
{
  Entities::Actor* actor = new Entities::Actor();
  actor->setPosition(pos);
  actor->setScale(scale);
  actor->setRotation(rotation);
  actor->setModel(modelName);
  actor->setTexture(textureName);
  add(actor);
  return actor;
}

// boilerplate for adding an entity directly to the scene
// lets you fully customize your entity with different complexity levels
Entities::Entity* Scene::Scene::instanceCreate(std::string name, Vector3D position)
{
  Entities::Entity* actor = factory->createEntity(name, position);
  actor->transform->setPosition(position);
  add(actor);
  return actor;
}

Entities::Entity* Scene::Scene::instanceCreateExt(std::string name, Vector3D position, Vector3D scale, Vector3D rotation)
{
  Entities::Entity* actor = factory->createEntity(name, position);
  actor->transform->setScale(scale);
  actor->transform->setRotation(rotation);
  actor->transform->setPosition(position);
  actor->sprite->setModel(name);
  add(actor);
  return actor;
}

Entities::Actor* Scene::Scene::instanceCreateGeneral(std::string name, std::string model, std::string texture, Vector3D position, Vector3D scale, Vector3D rotation)
{
  Entities::Actor* actor = factory->createActor(name, position);
  actor->setScale(scale);
  actor->setRotation(rotation);
  actor->setModel(model);
  actor->setTexture(texture);
  add(actor);
  return actor;
}

// add an entity to the root node list; we'll eventually be using the scene hierarchy to do most of this
void Scene::Scene::add(Entities::Entity* entity)
{
  rootList->add(entity);
  entity->SetId(rootList->getSize());
}

void Scene::Scene::clear()
{
  globalList->clear();
  rootList->clear();
}

// pick an entity from a scene given an origin vector and a direction, finds the first one
Entities::Entity* Scene::Scene::RayPick(Vector3D origin, Vector3D dir)
{
  float closestDistance = FLT_MAX;
  Entities::Entity* closestEntity = nullptr;

  for (const auto& entity : rootList->getEntities())
  {
    if (!entity->isVisible())
      continue;

    Components::BoundingBox* box = getComponentOfType(BoundingBox,entity);
    if (!box)
      continue;

    if ((box->min == 0 && box->max == 0))
      continue;

    float distance;
    if (Vector3D::RayIntersectsBoundingBox(origin, dir, box, distance))
    {
      if (distance < closestDistance)
      {
        closestDistance = distance;
        closestEntity = entity;
      }
    }
  }

  return closestEntity;
}

