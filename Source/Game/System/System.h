/*
/
// filename: System.h
// author: Callen Betts
// brief: defines the base system class
/
*/

#pragma once

namespace Input { class InputSystem; }
namespace Scene { class Scene; class SceneSystem; }

namespace Systems
{

  class System
  {

  public:

    System(std::string systemName);

    // each system is initialized, updated, or rendered
    virtual void init() {};
    virtual void update() {};
    virtual void render() {};

    // get the system's name
    const std::string getName() { return name; }

    bool IsSimulation() { return isSimulation; }
    void SetAsSimulation(bool val) { isSimulation = val; }

    // all systems have access to core engine and other important systems
    Engine::GlowEngine* engine;
    Input::InputSystem* input;
    Scene::SceneSystem* sceneSystem;

  protected:

    std::string name;
    bool isSimulation = false;

  };

}