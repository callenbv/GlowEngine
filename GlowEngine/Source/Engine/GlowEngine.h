/*
/
// filename: GlowEngine.h
// author: Callen Betts
// brief: defines GlowEngine class
/
*/

#pragma once

// forward declare systems
namespace Graphics { class Renderer; class Window; }
namespace Visual { class Camera; }
namespace Models { class ModelLibrary; }
namespace Textures { class TextureLibrary; }
namespace Materials { class MaterialLibrary; }
namespace Scene { class SceneSystem; }
namespace Meshes { class MeshLibrary; }
namespace Entities { class EntityFactory; }
namespace Audio { class SoundSystem; class SoundLibrary; }

namespace Engine
{
  
  class GlowEngine
  {

  public:
    GlowEngine();

    // handle engine states
    bool start();
    bool run();
    void stop();

    void update();
    void render();
    void exit();

    // create core systems
    void createCoreSystems();
    // create systems dependent on core systems
    void createLaterSystems();
    // create later core pointers
    void initializeSystemCorePointers();
    // setup global variables and data structures
    void SetupGlobalDataStructures();

    // cleanup on engine exit
    void cleanUp();

    // get system pointers
    Input::InputSystem* getInputSystem() { return input; }
    Graphics::Renderer* getRenderer() { return renderer; }
    Visual::Camera* getCamera() { return camera; }
    Models::ModelLibrary* getModelLibrary() { return modelLibrary; }
    Textures::TextureLibrary* getTextureLibrary() { return textureLibrary; }
    Materials::MaterialLibrary* getMaterialLibrary() { return materialLibrary; }
    Scene::SceneSystem* getSceneSystem() { return sceneSystem; }
    Graphics::Window* getWindow() { return window; }
    Meshes::MeshLibrary* getMeshLibrary() { return meshLibrary; }
    Entities::EntityFactory* getEntityFactory() { return factory; }
    Audio::SoundLibrary* getSoundLibrary() { return soundLibrary; }
    Audio::SoundSystem* getSoundSystem() { return soundSystem; }

    // get engine properties
    int getFps() { return fps; }
    int getTotalFrames() { return totalFrames; }
    bool isRunning() { return running; }
    float getDeltaTime() { return deltaTime; }

    // if we are playing
    bool isPlaying();
    bool InEditor() { return inEditor; }
    bool IsPaused() { return paused; }
    bool GameWindowIsFocused() { return gameWindowIsFocused; }
    void EnableEditor(bool val) { inEditor = val; }
    void setPlaying(bool val);
    void SetPaused(bool val) { paused = val; }
    void StopGame();
    void StartGame();
    void SetGameFocus(bool val) { gameWindowIsFocused = val; }

    // get the window handle from the window class
    HWND getWindowHandle();


  private:

    // system pointers
    Graphics::Renderer* renderer;
    Graphics::Window* window;
    Visual::Camera* camera;
    Input::InputSystem* input;
    Models::ModelLibrary* modelLibrary;
    Textures::TextureLibrary* textureLibrary;
    Materials::MaterialLibrary* materialLibrary;
    Meshes::MeshLibrary* meshLibrary;
    Scene::SceneSystem* sceneSystem;
    Entities::EntityFactory* factory;
    Audio::SoundLibrary* soundLibrary;
    Audio::SoundSystem* soundSystem;

    // core engine properties (fps, delta time)
    bool running;
    HWND windowHandle;

    // if we are in play mode
    bool playing;
    bool inEditor;
    bool paused;
    bool gameWindowIsFocused;

    // statistics
    float totalTime;
    float frameCount;
    float fpsTimer;
    int fps;
    int totalFrames;
    int frameTime;
    float deltaTime;

  };
}
