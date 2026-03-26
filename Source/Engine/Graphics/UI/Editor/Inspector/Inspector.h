/*
/
// filename: Inspector.h
// author: Callen Betts
// brief: defines Inspector class
/
*/

#pragma once
#include "Engine/Graphics/UI/Editor/Widget.h"

namespace Entities
{
  class Entity;
}

namespace Editor
{
  class Inspector : public Widget
  {

  public:

    Inspector(std::string title, std::string desc = "", ImGuiWindowFlags flags = 0);

    void update();
    void DragEntity();

    // the entity we want to inspect
    static void inspect(Entities::Entity* ent);

    static Entities::Entity* selectedEntity;
    static Entities::Entity* previousEntity;

  private:

    bool openContextMenu = false;
    void* selectedObject = nullptr;

  };
}