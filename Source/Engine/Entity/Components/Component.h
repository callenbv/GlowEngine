/*
/
// filename: Component.h
// author: Callen Betts
// brief: defines Component class
/
*/

#pragma once
#include "Property.h"

namespace Entities
{
  class Entity;
}

namespace Components
{

  // we have a container of all possible components

  class Component
  {
    public:

    enum ComponentType
    {
      Transform,
      Behavior,
      Sprite2D,
      Sprite3D,
      Animation3D,
      BoxCollider,
      Physics,
      Collider,
      BoundingBox,
      PlayerBehavior,
      None
    };

    public:

      Component();
      Component(const Component& other);

      void init();
      virtual void update() {};
      virtual void render() {};
      virtual void exit() {};
      virtual void load(const nlohmann::json&) {};
      virtual void display() {};
      virtual void CustomLoad(const nlohmann::json saveData) {};
      virtual void CustomSave(nlohmann::json& data) const {};

      virtual Components::Component* clone();

      const nlohmann::json Save() const;
      int getPriority() { return priority; }

      const ComponentType getType();

      // get our name
      std::string getName() { return name; }
      void SetName(std::string newName) { name = newName; }
      // get the parent
      Entities::Entity* getParent();
      // set the parent entity
      void setParent(Entities::Entity* newParent);
      // inspect in the editor
      virtual void inspect() {};

      Entities::Entity* parent;

      // add a variable to be modified in the editor
      void AddVariable(Variable var) { variables.push_back(var); }

      // get the variables vector
      std::vector<Variable>& getVariables() { return variables; }
      const std::vector<Variable>& getVariables() const { return variables; }
      // if this component can be simulated
      bool IsSimulation() { return simulation; }

    protected:
      
      // component identifiers
      std::string name;
      // type of component
      ComponentType type;
      // if this component should be updated
      bool active = true;
      // priority determines order of update
      int priority = 0;
      // if this component is simulated (updated when in editor or paused)
      bool simulation = true;

      // components have a list of variables we expose to the editor
      std::vector<Variable> variables;
      
  public:

    static std::map<Components::Component::ComponentType, Components::Component*> allComponents;

    static void AddNewComponent(Components::Component::ComponentType type, Components::Component* component);
    static std::map<Components::Component::ComponentType, Components::Component*>& GetComponentArchetypes();
  };

}

class ComponentFactory {
public:
  using CreatorFunc = Components::Component* (*)();

  static ComponentFactory& instance() {
    static ComponentFactory factory;
    return factory;
  }

  void registerComponent(const std::string& type, CreatorFunc func) {
    creators[type] = func;
    registeredTypes.push_back(type);
  }

  Components::Component* createComponent(const std::string& type) {
    auto it = creators.find(type);
    if (it != creators.end()) {
      return it->second();
    }
    return nullptr;
  }

  const std::vector<std::string>& getRegisteredTypes() const {
    return registeredTypes;
  }

private:
  std::unordered_map<std::string, CreatorFunc> creators;
  std::vector<std::string> registeredTypes;
};

template <typename T>
Components::Component* createInstance() {
  return new T();
}

#define REGISTER_COMPONENT(CLASS) \
    namespace { \
        inline bool register_##CLASS() { \
            ComponentFactory::instance().registerComponent(#CLASS, createInstance<Components::CLASS>); \
            return true; \
        } \
        static const bool registered_##CLASS = register_##CLASS(); \
    }

#define REGISTER_BEHAVIOR(CLASS) \
    namespace { \
        inline bool register_##CLASS() { \
            ComponentFactory::instance().registerComponent(#CLASS, createInstance<Game::CLASS>); \
            return true; \
        } \
        static const bool registered_##CLASS = register_##CLASS(); \
    }
