/*
/
// filename: EntityList.cpp
// author: Callen Betts
// brief: implements EntityList.h
//
// description: Entity lists let us create containers of entities which will be rendered and 
// updated each frame.
/
*/

#include "stdafx.h"
#include "EntityList.h"
#include "Engine/Entity/Entity.h"
#include "Engine/GlowEngine.h"
#include "Game/Scene/Scene.h"
#include "Game/Scene/SceneSystem.h"
#include <algorithm>

// base constructor
Entities::EntityList::EntityList(std::string newName)
:
  size(0),
  name(newName),
  destroyed(false)
{
  
}

// add a given entity to the list
void Entities::EntityList::add(Entities::Entity* entity)
{
  // ensure no duplicate named entities
  if (entity->getName() == "Entity")
  {
    entity->setName("Entity"+std::to_string(activeList.size()));
  }

  activeList.push_back(entity);
  size++;
}

void Entities::EntityList::insert(Entities::Entity* entity, int index)
{
  index = std::clamp(index, 0, (int)activeList.size());

  auto it = activeList.begin() + index;
  activeList.insert(it, entity);
  size++;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
nlohmann::json Entities::EntityList::Save()
{
  nlohmann::json saveData;

  for (auto& list : subLists)
  {
    list->Save();
  }

  // Iterate over our entites and add their data to the json object
  for (const auto& entity : activeList)
  {
    if (!entity->IsLocked())
    {
      saveData[entity->getName()] = entity->Save();
    }
  }

  return saveData;
}

/// <summary>
/// Find a sublist given a name
/// </summary>
/// <param name="name"> The name of the list to search for </param>
/// <returns> The list </returns>
Entities::EntityList* Entities::EntityList::FindSublist(std::string name)
{
  if (this->name == name)
  {
    return this;
  }

  for (const auto& list : subLists)
  {
    if (list->name == name)
    {
      return list;
    }
  }

  return nullptr;
}

/// <summary>
/// Deletes all entities, then deletes the list
/// </summary>
void Entities::EntityList::DestroySublists()
{
  for (auto& list : subLists)
  {
    list->Destroy();
  }
}

/// <summary>
/// Deletes a list and all of its entities
/// </summary>
void Entities::EntityList::Destroy()
{
  for (auto& entity : activeList)
  {
    entity->destroy();
  }

  destroyed = true;
}

/// <summary>
/// Deletes all entities in a list without deleting the list
/// </summary>
void Entities::EntityList::DeleteEntities()
{
  for (auto& entity : activeList)
  {
    entity->destroy();
  }
}

/// <summary>
/// Deletes all entities and ones in their sublists
/// </summary>
void Entities::EntityList::DeleteAllEntities()
{
  for (auto& list : subLists)
  {
    list->DeleteAllEntities();
  }

  DeleteEntities();
}

// update a list of entities
void Entities::EntityList::update()
{
  // update our parent scene to the current scene
  parentScene = EngineInstance::getEngine()->getSceneSystem()->getCurrentScene();

  for (auto it = activeList.begin(); it != activeList.end(); )
  {
    // check for destroyed entities
    if ((*it)->isDestroyed())
    {
      destroyList.push_back(*it);

      // erase the entity from the active list and get the new iterator position
      it = activeList.erase(it);
    }
    else
    {
      // add colliders to a separate list for update
      Components::Collider* col = getComponentOfType(Collider, (*it));

      if (col)
      {
        if (!col->isStatic())
        {
          parentScene->getGlobalList()->getNonStaticList().push_back((*it));
        }
        parentScene->getGlobalList()->getColliderList().push_back((*it));
      }

      // update the entity and increment the iterator
      (*it)->update();
      ++it;
    }
  }

  // destroy entities
  for (auto& entity : destroyList)
  {
    entity->~Entity();
    delete entity;
  }
  destroyList.clear();

  // recursively update any of our sublists
  for (auto& list : subLists)
  {
    list->update();
  }

  // Delete ourself if we are marked for destroy
  if (destroyed)
  {
    delete this;
  }
}

// this function is used to update our global list and check for collisions
void Entities::EntityList::updateColliders()
{
  if (colliderList.empty())
    return;

  checkCollisions();

  colliderList.clear();
  nonStaticList.clear();
}

// update a list of entities
void Entities::EntityList::render()
{
  // render all entities in the list
  for (auto entity : activeList)
  {
    if (!entity->isVisible())
      continue;

    entity->render();
  }

  for (auto& list : subLists)
  {
    list->render();
  }
}

void Entities::EntityList::clear()
{
  for (auto& list : subLists)
  {
    list->clear();
  }
  activeList.clear();
  subLists.clear();
}

// removes an entity from the list and deletes it
void Entities::EntityList::remove(Entities::Entity* entityToRemove)
{
  auto it = std::find_if(activeList.begin(), activeList.end(),
    [entityToRemove](Entities::Entity* entity) {
      return entity == entityToRemove;
    });
  if (it != activeList.end()) {
    activeList.erase(it);
  }
}

void Entities::EntityList::checkCollisions()
{
  for (auto it1 = nonStaticList.begin(); it1 != nonStaticList.end(); ++it1)
  {
    Entity* ent1 = *it1;
    Components::Collider* collider1 = getComponentOfType(Collider, ent1);

    for (auto it2 = colliderList.begin(); it2 != colliderList.end(); ++it2)
    {
      Entity* ent2 = *it2;

      // don't collide with ourselves; this seems strange, but this allows us to have absurd amounts
      // of static colliders with almost zero drawback
      if (ent2 == ent1)
        continue;

      Components::Collider* collider2 = getComponentOfType(Collider, ent2);

      // if both colliders are static, then we don't want to do any collision logic
      // this reduces our collision to O(N) complexity  
      if (collider2->isStatic() && collider1->isStatic()) 
        break;

      // detect collisions
      if (collider1->isColliding(collider2)) 
      {
        collider2->updateCollision(collider1);
        collider1->updateCollision(collider2);
      }
      else
      {
        if (collider1->getCollidingObjects().find(collider2) != collider1->getCollidingObjects().end() 
          && collider1->hasCollided() 
          && collider2->hasCollided())
        {
          collider2->leaveCollision(collider1);
          collider1->leaveCollision(collider2);
        }
      }
    }
  }
  colliderList.clear();
  nonStaticList.clear();
}

Entities::Entity* Entities::EntityList::find(std::string name)
{
  for (auto entity : activeList)
  {
    if (entity->getName() == name)
    {
      return entity;
    }
  }
  return nullptr;
}


// given a source index and a destination index, we move two entities within a list
void Entities::EntityList::ReorderEntities(int srcIndex, int dstIndex)
{
  auto entity = activeList[srcIndex];
  activeList.erase(activeList.begin() + srcIndex);
  activeList.insert(activeList.begin() + dstIndex, entity);
}
