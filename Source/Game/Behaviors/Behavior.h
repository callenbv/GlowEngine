/*
/
// filename: Behavior.h
// author: Callen Betts
// brief: defines Behavior class
/
*/

#pragma once
#include "Engine/Entity/Entity.h"

namespace Game
{

  class Behavior : public Components::Component
  {

  public:

    Behavior();

    virtual void update() {};

  protected:

    Input::InputSystem* input;
    Engine::GlowEngine* engine;

  };

}