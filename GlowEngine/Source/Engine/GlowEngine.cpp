/*
/
// filename: GlowEngine.cpp
// author: Callen Betts
// brief: implements GlowEngine.h
/
*/

#include "stdafx.h"
#include "GlowEngine.h"
#include "Game/Scene/Scene.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Entity/Components/Visual/Models/ModelLibrary.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include "Engine/Graphics/Materials/MaterialLibrary.h"
#include "Game/Scene/SceneSystem.h"
#include "Engine/Graphics/Window/Window.h"
#include "Engine/Graphics/Meshes/MeshLibrary.h"
#include "Engine/Audio/SoundLibrary.h"
#include "Engine/Audio/SoundSystem.h"

// initialize engine values
Engine::GlowEngine::GlowEngine()
  :
  running(false),
  deltaTime(0),
  frameTime(0),
  renderer(nullptr),
  input(nullptr),
  modelLibrary(nullptr),
  sceneSystem(nullptr),
  textureLibrary(nullptr),
  window(nullptr),
  windowHandle(NULL),
  totalTime(0),
  frameCount(0),
  fpsTimer(0),
  totalFrames(0),
  inEditor(true),
  paused(true),
  playing(false),
  gameWindowIsFocused(false),
  fps(60)
{
  EngineInstance::setup(this);
}

// start the engine - returns false if failed, true on success
bool Engine::GlowEngine::start()
{
  // we need a window immediately
  window = new Graphics::Window();

  // create the window
  Logger::write("Starting engine...");

  // failed
  if (!window->setup())
  {
    Logger::error("Invalid window handle");
    return false;
  }

  // create early systems
  createCoreSystems();
  // create later systems
  createLaterSystems();
  // setup global data
  SetupGlobalDataStructures();
  // success
  Logger::write("Done!");
  return true;
}

// run the engine
bool Engine::GlowEngine::run()
{
  // update and display the window
  window->updateWindow();

  // calculate delta time
  LARGE_INTEGER frequency, prevTime, currTime;

  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&prevTime);

  // run the engine
  running = true;

  while (running)
  {
    // windows messages
    if (!window->updateMessages())
    {
      break;
    }

    // calculate the delta time
    QueryPerformanceCounter(&currTime);
    deltaTime = static_cast<float>(currTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
    prevTime = currTime;
    fpsTimer += deltaTime;
    frameCount++;
    totalFrames++;

    // update systems
    update();
    // render systems
    render();
    // finish render
    input->Clear();

    // update our FPS
    if (fpsTimer >= 1.0f)
    {
      fps = static_cast<int>(frameCount);
      frameCount = 0;
      fpsTimer -= 1.0f;
    }
  } 

  // stop running
  return window->getMessageParam();
}

// stop all processes in the engine and exit
void Engine::GlowEngine::stop()
{
  Logger::write("Stopping engine");
  running = false;
}

// loop through and update each system
void Engine::GlowEngine::update()
{
  // update all systems
  for (auto system : SystemInstance::getSystems())
  {
    // Systems that are simulations will still run when paused
    if (paused)
    {
      if (system->IsSimulation())
      {
        system->update();
      }
    }
    else
    {
      system->update();
    }
  }
}

// call the renderer updates and render systems
void Engine::GlowEngine::render()
{
  // start render frame update
  renderer->beginFrame();

  // render all systems
  sceneSystem->render();

  // renderer update
  renderer->update();

  // end renderer frame and present screen
  renderer->endFrame();
}

// exit the engine and cleanup
void Engine::GlowEngine::exit()
{
  cleanUp();

  sceneSystem->getCurrentScene()->SaveScene();
}

// create systems that need to be initialized first
void Engine::GlowEngine::createCoreSystems()
{
  // window handle
  windowHandle = getWindowHandle();
  // input
  input = new Input::InputSystem("InputSystem");
  // setup renderer
  renderer = new Graphics::Renderer(windowHandle);
  // camera, though it belongs to the renderer we have a quick pointer
  camera = renderer->getCamera();
  // audio
  soundLibrary = new Audio::SoundLibrary();
  soundSystem = new Audio::SoundSystem();
}

// create systems dependent on core systems
void Engine::GlowEngine::createLaterSystems()
{
    // material library
    materialLibrary = new Materials::MaterialLibrary();
    materialLibrary->load();
    // mesh library
    meshLibrary = new Meshes::MeshLibrary();
    meshLibrary->load();
    // model library
    modelLibrary = new Models::ModelLibrary();
    modelLibrary->load();
    // texture library
    textureLibrary = new Textures::TextureLibrary();
    textureLibrary->load();
    // entity factory
    factory = new Entities::EntityFactory();
    // scene system
    sceneSystem = new Scene::SceneSystem("SceneSystem");
    sceneSystem->init();
    sceneSystem->SetAsSimulation(true);
    // initialize later system core pointers
    initializeSystemCorePointers();
    // set the first scene
    sceneSystem->setCurrentScene("ForestScene");
}

// used to give core system pointers their 
void Engine::GlowEngine::initializeSystemCorePointers()
{
  for (const auto& system : SystemInstance::getSystems())
  {
    system->sceneSystem = sceneSystem;
  }
}

// this adds archetypes for components so we can a base component to be cloned
void Engine::GlowEngine::SetupGlobalDataStructures()
{
}

// called on engine exit
void Engine::GlowEngine::cleanUp()
{
  Logger::write("Cleaning up...");
}

bool Engine::GlowEngine::isPlaying()
{
    return playing;
}

void Engine::GlowEngine::setPlaying(bool val)
{
  playing = val;
}

/// <summary>
/// When the game stops, we revert back to our default state
/// Anything made while the game is running is deleted
/// </summary>
void Engine::GlowEngine::StopGame()
{
  setPlaying(false);
  sceneSystem->getCurrentScene()->LoadSnapshot();
}

/// <summary>
/// When the game starts, we initialize the scene (runs scene init)
/// </summary>
void Engine::GlowEngine::StartGame()
{
  setPlaying(true);
  sceneSystem->getCurrentScene()->SaveSnapshot();
  sceneSystem->getCurrentScene()->init();
}

// directly get the window handle
HWND Engine::GlowEngine::getWindowHandle()
{
  return window->getHandle();
}
