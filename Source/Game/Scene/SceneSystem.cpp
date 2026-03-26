/*
/
// filename: SceneSystem.cpp
// author: Callen Betts
// brief: implements SceneSystem.h
/
*/

#include "stdafx.h"
#include "SceneSystem.h"
#include "ForestScene/ForestScene.h"

Scene::SceneSystem::SceneSystem(std::string systemName) 
  : System(systemName)
{
  addScene("ForestScene");
  currentScene = nullptr;
}

// update all entities within the scene
void Scene::SceneSystem::update()
{
  currentScene->update();
  currentScene->updateEntities();

  // restart the current scene
  if (input->keyTriggered('R'))
  {
    // currentScene->restart();
  }
}

// render all the entities within a scene
void Scene::SceneSystem::render()
{
  currentScene->render();
}

// initialize the scene system
void Scene::SceneSystem::init()
{
  
}

// set the current scene
void Scene::SceneSystem::setCurrentScene(Scene* scene)
{
  // check if the scene was valid first
  if (!scene)
  {
    Logger::error("Scene " + scene->getName() + "was invalid");
    return;
  }
  if (currentScene)
  {
    // exit the scene
    currentScene->exit();
  }

  // set the current scene
  currentScene = scene;
  currentScene->LoadScene();
}

void Scene::SceneSystem::setCurrentScene(std::string name)
{
  setCurrentScene(scenes[name]);
}

// exit the current scene
void Scene::SceneSystem::exitCurrentScene()
{
  currentScene->exit();
}

// add a new scene to the map of scenes
void Scene::SceneSystem::addScene(std::string sceneName)
{
  scenes[sceneName] = new ForestScene();
}
