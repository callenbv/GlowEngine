/*
/
// filename: EntityFactory.h
// author: Callen Betts
// brief: defines base EntityFactory class
/
*/

#pragma once

#include "Components/Component.h"

namespace Entities
{

  class EntityFactory : public Systems::System
  {

  public:

    // initialize as a system with core pointers
    EntityFactory(std::string directoryPath = "Data/Entities");

    // add an entity to the archetype map
    void addArchetype(std::string name, std::string filePath);

    // load an entity's data
    Entities::Entity* loadEntity(std::string name);
    std::map<std::string, Entities::Entity*>& GetArchetypes() { return archetypes; }

    // create an entity and add it to the active scene (using loadEntity())
    Entities::Entity* createEntity(std::string name, Vector3D position, EntityType type = EntityType::Entity);
    Entities::Actor* createActor(std::string name, Vector3D position, EntityType type = EntityType::Entity);

    // Construct an base entity actor
    static Entities::Actor* CreateBaseActor(std::string name="Actor", Vector3D position = {0});
    static Entities::Entity* CreateBaseEntity(std::string name="Entity", Vector3D position = {0});

  private:

    // map of names to entity archetypes
    std::map<std::string, Entities::Entity*> archetypes;
  };
}