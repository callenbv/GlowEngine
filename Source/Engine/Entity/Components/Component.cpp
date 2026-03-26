/*
/
// filename: Component.cpp
// author: Callen Betts
// brief: implements Component.h
/
*/

#include "stdafx.h"
#include "Component.h"
#include "Engine/Entity/Entity.h"

// base component constructor
std::map<Components::Component::ComponentType, Components::Component*> Components::Component::allComponents;

Components::Component::Component()
{
  init();
}

Components::Component::Component(const Component& other)
{
  variables.clear();
  init();
}

void Components::Component::init()
{
  type = ComponentType::None;
  name = "Component";
  parent = nullptr;
}

// clone is more flexible than copy constructor
Components::Component* Components::Component::clone()
{
  return new Components::Component(*this);
}

// Save all properties of a component and return its json data
const nlohmann::json Components::Component::Save() const
{
  nlohmann::json saveData;

  // Save any specific fields for components
  CustomSave(saveData);

  for (auto variable : variables)
  {
    saveData[variable.name] = variable.Save();
  }

  return saveData;
}

// get the type
const Components::Component::ComponentType Components::Component::getType()
{
  return type;
}

// get the parented entity
Entities::Entity* Components::Component::getParent()
{
  return parent;
}

void Components::Component::setParent(Entities::Entity* newParent)
{
  parent = newParent;
}

void Components::Component::AddNewComponent(Components::Component::ComponentType type, Component* component)
{
  allComponents[type] = component;
}

std::map<Components::Component::ComponentType, Components::Component*>& Components::Component::GetComponentArchetypes()
{
  return allComponents;
}
