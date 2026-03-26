/*
/
// filename: GameWindow.cpp
// author: Callen Betts
// brief: implements GameWindow.h
/
*/

#include "stdafx.h"
#include "GameWindow.h"
#include "Engine/Graphics/Window/Window.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/UI/Editor/Inspector/Inspector.h"
#include "Engine/Graphics/Camera/Camera.h"
#include "Game/Scene/Scene.h"
#include "Game/Scene/SceneSystem.h"

// when we are focused on our game window, we can click on objects to select them
void Editor::GameWindow::update()
{
  // calculate the size of the game window
  calculateGameWindowSize();

  // if we're not in focus, then we shouldn't have anything selected; focus is not playing, it is if the game window is selected
  EngineInstance::getEngine()->SetGameFocus(ImGui::IsWindowFocused());

  // render the game to an ImGui texture
  ID3D11ShaderResourceView* texture = EngineInstance::getEngine()->getRenderer()->GetGameTexture();
  if (texture)
  {
    ImGui::Image((void*)texture, availableSize);
  }

  // while we're playing
  if (EngineInstance::IsPlaying())
  {
    DrawCrosshair();
  }
}

// grab an entity by casting a ray into the scene based on our mouse coordinates, and then return that entity
Entities::Entity* Editor::GameWindow::RayPickEntity(Vector3D worldCoords, DirectX::XMMATRIX view)
{
  Visual::Camera* camera = EngineInstance::getEngine()->getCamera();

  Vector3D rayOrigin = Vector3D::XMVectorToVector3D(camera->getPosition());
  Vector3D rayDirection = worldCoords - rayOrigin; // Compute the direction of the ray

  rayDirection.normalize();

  // Perform ray picking in the scene
  Entities::Entity* pickedEntity = EngineInstance::getEngine()->getSceneSystem()->getCurrentScene()->RayPick(rayOrigin, rayDirection);
  return pickedEntity;
}

// calculate the size of our game window
void Editor::GameWindow::calculateGameWindowSize()
{
  // calculate the aspect ratio
  const float aspectRatio = EngineInstance::getEngine()->getWindow()->getAspectRatio();

  availableSize = ImGui::GetContentRegionAvail();
  ImVec2 start = ImGui::GetCursorScreenPos();
  ImVec2 end = ImVec2(start.x + availableSize.x, start.y + availableSize.y);

  // Calculate the new size to maintain the aspect ratio
  float newWidth = availableSize.x;
  float newHeight = availableSize.x / aspectRatio;

  if (newHeight > availableSize.y)
  {
    newHeight = availableSize.y;
    newWidth = availableSize.y * aspectRatio;
  }

  // Set the new size of the ImGui window based on the aspect ratio
  ImVec2 newSize(newWidth, newHeight);

  // Update the size after setting the new window size
  mouseIsInsideWindow = ImGui::IsMouseHoveringRect(start,end);
  availableSize = newSize;
  gameScreenPosition = ImGui::GetCursorScreenPos();

}

void Editor::GameWindow::DrawCrosshair()
{
  // draw a crosshair at the middle of our window
  ImGuiIO& io = ImGui::GetIO();
  ImDrawList* draw_list = ImGui::GetForegroundDrawList();

  // Screen dimensions
  float screenWidth = availableSize.x;
  float screenHeight = availableSize.y;

  // Crosshair properties
  float centerX = gameScreenPosition.x + screenWidth * 0.5f;
  float centerY = gameScreenPosition.y + screenHeight * 0.5f;
  float crosshairSize = 10.0f; // Size of the crosshair lines
  ImU32 color = IM_COL32(255, 255, 255, 255); // White color

  // Draw vertical line
  draw_list->AddLine(ImVec2(centerX, centerY - crosshairSize), ImVec2(centerX, centerY + crosshairSize), color, 2.0f);

  // Draw horizontal line
  draw_list->AddLine(ImVec2(centerX - crosshairSize, centerY), ImVec2(centerX + crosshairSize, centerY), color, 2.0f);
}

// return if we're inside the game window or not
bool Editor::GameWindow::mouseIsInsideWindow = false;

bool Editor::GameWindow::MouseIsInsideWindow()
{
  return mouseIsInsideWindow;
}
