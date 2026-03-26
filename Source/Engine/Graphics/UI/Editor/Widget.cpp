/*
/
// filename: Widget.cpp
// author: Callen Betts
// brief: implements Widget.h
/
*/

#include "stdafx.h"
#include "Widget.h"

Editor::Widget::Widget(std::string title_, std::string desc, ImGuiWindowFlags flags_)
  :
  title(title_),
  description(desc),
  flags(flags_)
{

}

// main update frame, calls render of all elements
void Editor::Widget::renderFrame()
{
  if (!loaded)
  {
    init();
    loaded = true;
  }

  beginFrame();
  update();
  endFrame();
}

// called on all widgets
void Editor::Widget::beginFrame()
{
  if (borderless)
  {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  }
  ImGui::Begin(title.c_str(), nullptr, flags);
}

void Editor::Widget::endFrame()
{
  ImGui::End();
  if (borderless)
  {
    ImGui::PopStyleVar(2);
  }
}
