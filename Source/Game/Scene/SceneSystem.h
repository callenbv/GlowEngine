/*
/
// filename: SceneSystem.h
// author: Callen Betts
// brief: defines the scene system class
/
*/

#pragma once
#include "Game/Scene/Scene.h"

namespace Scene
{

  class SceneSystem : public Systems::System
  {

  public:

    // constructor
    SceneSystem(std::string systemName);

    // update the entities in all scenes
    void update();
    // render the entities in all scenes
    void render();
    // initialize
    void init();

    // set the current scene
    void setCurrentScene(Scene* scene);
    // set the current scene given a name
    void setCurrentScene(std::string name);

    // exit a scene
    void exitCurrentScene();

    // retrieve the current scene
    Scene* getCurrentScene() { return currentScene; }

    // add a new scene to the map of scenes
    void addScene(std::string sceneName);

  private:

    // the scene that is currently active
    Scene* currentScene;

    // we have a vector of scenes that we iteratively update
    std::map<std::string, Scene*> scenes;
    std::vector<Scene*> preRegisteredScenes;

  };

}