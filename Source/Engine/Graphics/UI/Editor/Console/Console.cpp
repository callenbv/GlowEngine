/*
/
// filename: stub.cpp
// author: Callen Betts
// brief: implements stub.h
/
*/

#include "stdafx.h"
#include "Console.h"
#include "Engine/GlowEngine.h"

void Editor::Console::update()
{
  // print out all of the console messages
  for (const auto& message : Logger::getMessages())
  {
    ImGui::TextUnformatted(message.c_str());
  }

  // padding at the bottom
  ImGui::NewLine();
  ImGui::NewLine();

  // scroll to the new message
  if (Logger::AddedNewMessage())
  {
    ImGui::SetScrollHereY(1.0f);
    Logger::SetNewMessage(false);
  }
}
