/*
/
// filename: stub.cpp
// author: Callen Betts
// brief: implements stub.h
/
*/

#include "stdafx.h"
#include "EngineInspector.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"
#include "Game/Scene/Scene.h"
#include "Game/Scene/SceneSystem.h"

void Editor::EngineInspector::update()
{
  Graphics::Renderer* renderer = EngineInstance::getEngine()->getRenderer();
  Engine::GlowEngine* engine = EngineInstance::getEngine();

  // write the FPS values to the console
  ImGui::Text(("FPS: " + std::to_string(engine->getFps())).c_str());
  ImGui::Text(("Delta Time: " + std::to_string(engine->getDeltaTime())).c_str());
  ImGui::Text(("Entities: " + std::to_string(engine->getSceneSystem()->getCurrentScene()->getEntityCount())).c_str());
}
