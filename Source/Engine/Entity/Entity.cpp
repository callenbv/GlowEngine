/*
/
// filename: Entity.cpp
// author: Callen Betts
// brief: implements Entity.h
/
*/

#include "stdafx.h"
#include "Entity.h"
#include <utility>

// base entity constructor
Entities::Entity::Entity(std::string name)
  :
  id(0),
  name(name),
  destroyed(false)
{
  init();
}

// copy constructor for entity
Entities::Entity::Entity(const Entity& other)
  :
  id(0),
  name(other.name),
  destroyed(false),
  visible(other.visible)
{
  // copy components
  for (const auto& component : other.components)
  {
    Components::Component* comp = component->clone();
    addComponent(comp);
  }

  // update quick pointers
  transform = getComponentOfType(Transform, this);
  sprite = getComponentOfType(Sprite3D, this);
  physics = getComponentOfType(Physics, this);

  init();
}

void Entities::Entity::init()
{
}

/// <summary>
/// Save all of an entity's data by serializing it into json
/// </summary>
/// <returns> A non modifiable json object </returns>
const nlohmann::json Entities::Entity::Save() const
{
  nlohmann::json saveData;
  nlohmann::json componentData;

  // Add entity's properties to the json object
  for (auto variable : variables)
  {
    nlohmann::json varData = variable.Save();
    saveData[variable.name] = varData;
  }

  // Iterate over our components and add their data to the json object
  for (const auto& component : components)
  {
    componentData[component->getName()] = component->Save();
  }

  saveData["Components"] = componentData;

  return saveData;
}

/// <summary>
/// Load data into an entity given json object
/// </summary>
/// <param name=""> Json object </param>
void Entities::Entity::Load(const nlohmann::json data)
{
  // Check if the "Components" key exists in the JSON data
  if (data.contains("Components"))
  {
    // Get the components data
    const auto& componentsData = data["Components"];

    // Iterate over all components in the JSON data
    for (auto& [componentName, componentData] : componentsData.items())
    {
      // Create the component using the factory
      Components::Component* component = ComponentFactory::instance().createComponent(componentName);

      // If the component creation failed, handle it (e.g., log an error)
      if (component == nullptr)
      {
        Logger::write("Failed to create component: " + componentName);
        continue;
      }

      component->setParent(this);

      // Iterate over each variable in the component's JSON data
      for (auto& [variableName, variableData] : componentData.items())
      {
        // Find the variable in the component's variable list
        for (auto& variable : component->getVariables())
        {
          if (variable.name == variableName)
          {
            // The variable data is stored under "value"
            auto valueData = variableData["value"];

            // Use std::visit to assign the value to the variable based on its type
            std::visit([&valueData](auto&& arg) {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<T, int*>)
              {
                *arg = valueData.get<int>();  // Assign the int value
              }
              else if constexpr (std::is_same_v<T, float*>)
              {
                *arg = valueData.get<float>();  // Assign the float value
              }
              else if constexpr (std::is_same_v<T, bool*>)
              {
                *arg = valueData.get<bool>();  // Assign the bool value
              }
              else if constexpr (std::is_same_v<T, std::string*>)
              {
                *arg = valueData.get<std::string>();  // Assign the string value
              }
              else if constexpr (std::is_same_v<T, Vector3D*>)
              {
                // Assign the Vector3D value from the JSON object
                arg->x = valueData["x"].get<float>();
                arg->y = valueData["y"].get<float>();
                arg->z = valueData["z"].get<float>();
              }
              }, variable.value);
          }
        }
      }

      // Add individual component data
      component->CustomLoad(componentData);

      // Add the component to the entity's components list
      components.push_back(component);
    }
  }
}

// virtual destructor for entities
Entities::Entity::~Entity()
{
  for (auto component : components)
  {
    delete component;
  }
}

void Entities::Entity::load(const nlohmann::json&)
{
}

// update all the components of an entity
void Entities::Entity::update()
{
  for (auto& component : components)
  {
    if (EngineInstance::IsPaused())
    {
      // some components (physics, colliders) don't get updated or simulated in editor
      if (component->IsSimulation())
      {
        component->update();
      }
    }
    else
    {
      // We are playing, so we update every component
      component->update();
    }
  }
}

// render an entity's components - typically this is a Sprite3D
void Entities::Entity::render()
{
  // get the sprite3D to render
  for (auto component : components)
  {
    component->render();
  }
}

// attach a new component to an entity
void Entities::Entity::addComponent(Components::Component* component)
{
  // add the components
  component->setParent(this);
  this->components.push_back(component);

  // sort them in order of priority, higher priority goes first
  std::sort(components.begin(), components.end(), [](Components::Component* a, Components::Component* b) {
    return a->getPriority() > b->getPriority();
  });
}

// flag an entity for destroy
void Entities::Entity::destroy()
{
  if (locked)
  {
    return;
  }

  destroyed = true;
}

void Entities::Entity::setName(std::string newName)
{
  name = newName;
}

// has a component
bool Entities::Entity::hasComponent(Components::Component::ComponentType type)
{
  for (Components::Component* component : components)
  {
    if (component->getType() == type)
    {
      return true;
    }
  }
  return false;
}

void Entities::Entity::DeleteComponent(Components::Component* component)
{
  // Find the component in the components vector
  auto it = std::find(components.begin(), components.end(), component);

  // If the component is found, delete it and remove it from the vector
  if (it != components.end())
  {
    delete* it;
    components.erase(it);
  }
}

// get a component
Components::Component* Entities::Entity::getComponent(Components::Component::ComponentType type)
{
  for (Components::Component* component : components)
  {
    if (component->getType() == type)
    {
      return component;
    }
  }
  return nullptr;
}

bool Entities::Entity::hasComponent(const std::string& type)
{
  for (const auto& comp : components)
  {
    if (comp->getName() == type)
    {
      return true;
    }
  }
  return false;
}

void Entities::Entity::addComponent(const std::string& type)
{
  // make sure we don't have this component already
  if (!hasComponent(type)) 
  {
    Components::Component* component = ComponentFactory::instance().createComponent(type);
    if (component) 
    {
      component->parent = this;
      components.push_back(component);
    }
  }
}

