/*
/
// filename: Entity.h
// author: Callen Betts
// brief: defines base entity class
/
*/

#pragma once

#include "Components/Component.h"
#define getComponentOfType(type, entity) dynamic_cast<Components::type*>(entity->getComponent(Components::Component::ComponentType::type##))

namespace Entities
{

  class Entity
  {

  public:

    Entity(std::string name = "Entity");
    Entity(const Entity& other);

    virtual ~Entity();
    virtual void load(const nlohmann::json&);

    void init(); 

    // Return the serialized data of the entity as a json
    const nlohmann::json Save() const;
    void Load(const nlohmann::json);

    // update all of an entity's components
    void update();
    // render an entity's components
    void render();
    // add a new component
    void addComponent(Components::Component* component);
    // destroy an entity
    void destroy();
    // return if destroyed
    bool isDestroyed() { return destroyed; }
    // if we are selected by the inspector
    bool IsSelected() { return selected; }
    // if visible
    bool isVisible() { return visible; }
    // get the name
    std::string getName() { return name; }
    // set name
    void setName(std::string newName);
    // has a component
    bool hasComponent(Components::Component::ComponentType type);
    // delete a component
    void DeleteComponent(Components::Component* component);
    // get a component
    Components::Component* getComponent(Components::Component::ComponentType type);
    // get the components vector
    std::vector<Components::Component*> getComponents() { return components; }
    std::vector<Variable>& getVariables()  { return variables; }

    // add a variable to be modified in the editor
    void AddVariable(Variable var) { variables.push_back(var); }
    // set selected
    void SetSelected(bool val) { selected = val; }
    // toggle visibility
    void ToggleVisiblity() { visible = !visible; }
    // set locked status
    bool IsLocked() { return locked; }
    // get the ID
    int GetId() { return id; }
    // set the ID
    void SetId(int val) { id = val; }

    bool hasComponent(const std::string& type);

    void addComponent(const std::string& type);

    // core components are public for easy modification and access
    Components::Transform* transform;
    Components::Sprite3D* sprite;
    Components::Physics* physics;
    Components::BoundingBox* boundingBox;

  protected:

    int id;
    bool destroyed;
    bool visible = true;
    bool selected = false; // if we are selected by the inspector
    bool needsComponentSort = true; // flag for sorting components
    bool locked = false; // locked entities cannot be edited

    std::string name;

    std::vector<Components::Component*> components; // entity component list
    std::vector<Variable> variables;

  private:

  };

}