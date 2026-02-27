/*
/
// filename: EntityList.h
// author: Callen Betts
// brief: defines base EntityList class as a container of entities
/
*/

#pragma once

namespace Entities
{
  class Entity;
  class EntityList;

  // define a wrapper for entity lists
  class EntityListWrapper
  {

  public:

    EntityListWrapper(Entities::EntityList* newList = nullptr) { list = newList; name = "Wrapper"; }

    // pointer to our list
    Entities::EntityList* list;

    // if the list is collapsed
    bool collapsed = true;
    // if the container is visible or active
    bool active = true;
    // name of wrapper
    std::string name;

  };

  // define a wrapper for entities
  class EntityWrapper
  {

  public:

    EntityWrapper(Entities::Entity* ent) :
        entity(ent),
        parent(nullptr)
    {

    }

    EntityWrapper() :
        parent(nullptr),
        entity(nullptr)
    {

    }

    Entities::Entity* entity;
    EntityListWrapper* parent;
  };


  class EntityList
  {

  public:

    EntityList(std::string name = "Container");

    void add(Entities::Entity* entity);
    void update();
    void updateColliders();
    void render();
    void clear();
    void remove(Entities::Entity* entity);
    void checkCollisions();
    void insert(Entities::Entity* entity, int index);
    nlohmann::json Save();
    Entities::EntityList* FindSublist(std::string name);

    // Used to destroy lists and delete entities en masse
    void DestroySublists();
    void Destroy();
    void DeleteEntities();
    void DeleteAllEntities();

    Entities::Entity* find(std::string name);

    int getSize() { return size; }
    std::string getName() { return name; }
    void SetName(std::string name_) { name = name_; }

    // retrieve the active list; one returns const while one does not, so our scene editor can modify it
    std::vector<Entities::Entity*>& getEntities() { return activeList; }
    const std::vector<Entities::Entity*>& getEntities() const { return activeList; }

    std::vector<Entities::Entity*>& getColliderList() { return colliderList; }
    std::vector<Entities::Entity*>& getNonStaticList() { return nonStaticList; }
    std::vector<Entities::EntityList*>& getSubLists() { return subLists; }

    // reorder the entities by id
    void ReorderEntities(int srcIndex, int dstIndex);

  private:

    // vectors of lists for handling updates and collisions
    std::vector<Entities::Entity*> activeList;
    std::vector<Entities::Entity*> destroyList;

    std::vector<Entities::Entity*> colliderList;
    std::vector<Entities::Entity*> nonStaticList;

    std::vector<Entities::EntityList*> subLists; // used for scene hierarchy drawing

    // pointer to our parent scene
    Scene::Scene* parentScene = nullptr;

    // size of current active list
    int size;
    bool destroyed; // we can mark an entire list for delete

    // name of entity list for display purposes
    std::string name;

  };

}