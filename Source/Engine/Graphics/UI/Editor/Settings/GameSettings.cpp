/*
/
// filename: GameSettings.cpp
// author: Callen Betts
// brief: implements GameSettings.h
/
*/

#include "stdafx.h"
#include "GameSettings.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include "Engine/Graphics/UI/Editor/Inspector/Inspector.h"
#include "Engine/Graphics/Renderer.h"
#include "Game/Scene/SceneSystem.h"
#include "Game/Scene/Scene.h"

Editor::GameSettings::GameSettings()
{
  engine = EngineInstance::getEngine();
}

void Editor::GameSettings::update()
{
  ID3D11ShaderResourceView** playButtonTexture = EngineInstance::getEngine()->getTextureLibrary()->get("PlayButton")->getTextureView();
  ID3D11ShaderResourceView** pauseTexture = EngineInstance::getEngine()->getTextureLibrary()->get("PauseButton")->getTextureView();
  ID3D11ShaderResourceView** stopTexture = EngineInstance::getEngine()->getTextureLibrary()->get("StopButton")->getTextureView();

  sceneSystem = engine->getSceneSystem();

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

  // draw our play/pause button
  // if we are in editor, click to play the game
  if (engine->IsPaused())
  {
    if (ImGui::ImageButton((void*)(*playButtonTexture), { 64,64 }))
    {
      OnPlay();
    }
  }
  else
  {
    // if we are not in editor, click to pause the game
    if (ImGui::ImageButton((void*)(*pauseTexture), { 64,64 }))
    {
      OnPause();
    }
  }

  // draw our stop button which will enable editor and restart the scene to its default state
  ImGui::SameLine();
  if (engine->isPlaying())
  {
    if (ImGui::ImageButton((void*)(*stopTexture), { 64,64 }))
    {
      OnStop();
    }
  }

  ImGui::PopStyleVar();
  ImGui::PopStyleColor(1);
}

// called when the play button is hit; restarts the current active scene and sets our focus
void Editor::GameSettings::OnPlay()
{
  if (!engine->isPlaying())
  {
    // first launch logic
    engine->SetPaused(false);
    engine->StartGame();
  }
  else
  {
    // resume logic
    engine->SetPaused(false);
  }
}

// called when we pause our game 
void Editor::GameSettings::OnPause()
{
  // if we're playing the game, pause by going into the editor
  engine->SetPaused(true);
}

// called when we click the stop button
void Editor::GameSettings::OnStop()
{
  engine->SetPaused(true);
  engine->StopGame();
}
