/*
/
// filename: Scene.h
// author: Callen Betts
// brief: defines base scene class
/
*/

#pragma once
#include "Engine/Entity/EntityList/EntityList.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityFactory.h"
#include "Engine/GlowEngine.h"

namespace Scene
{

  class Scene
  {

  public:

    Scene();

    virtual void init() {};
    virtual void update() {};

    void SaveSnapshot(std::string filePath = "Data/Temp/Scene_Data.json");
    void LoadSnapshot(std::string filePath = "Data/Temp/Scene_Data.json");

    void SaveScene();
    void LoadScene();

    void exit();
    void restart();

    virtual void render();

    void updateEntities();
    void renderEntities();

    std::string getName() { return name; }

    // create an entity directly to this scene with already made components
    Entities::Actor* createEntity(Vector3D pos, Vector3D scale, Vector3D rotation, std::string modelName, std::string textureName = "");

    // create an entity and add it to the scene
    Entities::Entity* instanceCreate(std::string name, Vector3D position);
    Entities::Entity* instanceCreateExt(std::string name, Vector3D position, Vector3D scale, Vector3D rotation = { 0 });
    Entities::Actor* instanceCreateGeneral(std::string name, std::string model, std::string texture, Vector3D position, Vector3D scale, Vector3D rotation = {0});

    // add an entity to the scene (allows us to make entities from code easily)
    void add(Entities::Entity* entity);
    // clear all entities
    void clear();

    // get our global list
    Entities::EntityList* getGlobalList() { return globalList; }
    Entities::EntityList* getRootList() { return rootList; }
    int getEntityCount() { return (int)rootList->getEntities().size(); }

    // cast a ray and grab an entity from our scene
    Entities::Entity* RayPick(Vector3D origin, Vector3D dir);

  protected:

    // system pointers
    Engine::GlowEngine* engine;
    Input::InputSystem* input;

    std::string name;

    // entity list of all colliders
    Entities::EntityList* globalList;
    // dummy list that contains all entities for statistics
    Entities::EntityList* rootList;
    // entity factory
    Entities::EntityFactory* factory;

  };

}