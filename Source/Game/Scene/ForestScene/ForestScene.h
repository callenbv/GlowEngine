/*
/
// filename: ForestScene.h
// author: Callen Betts
// brief: defines ForestScene class
/
*/

#pragma once
#include "Game/Scene/Scene.h"

namespace Scene
{

  class ForestScene : public Scene
  {

  public:

    ForestScene() : Scene() {};

    void init();
    void update();
    void exit();

  };

}