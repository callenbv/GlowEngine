/*
/
// filename: GameWindow.h
// author: Callen Betts
// brief: defines GameWindow class
/
*/

#pragma once
#include "Engine/Graphics/UI/Editor/Widget.h"

namespace Editor
{
  class GameWindow : public Widget
  {

  public:

    // track if our mouse is in the window or not
    static bool MouseIsInsideWindow();

    // game window needs a lot more flags
    GameWindow(std::string title, std::string desc = "", ImGuiWindowFlags flags = 
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse) 
      : Widget(title, desc, flags)
    {
      borderless = true;
    }
    
    void update();
    void calculateGameWindowSize();
    void DrawCrosshair();

    Entities::Entity* RayPickEntity(Vector3D vector, DirectX::XMMATRIX view);

  private:

    // game window properties
    ImVec2 availableSize;
    ImVec2 gameScreenPosition;
    static bool mouseIsInsideWindow;

  };
}