/*
/
// filename: PlayerBehavior.h
// author: Callen Betts
// brief: defines PlayerBehavior class
/
*/

#pragma once
#include "Engine/Entity/Entity.h"
#include "Game/Behaviors/Behavior.h"

namespace Game
{

  class PlayerBehavior : public Game::Behavior
  {

  public:

    PlayerBehavior();

    virtual void update();

  private:

    float moveSpeed;
    float jumpSpeed;
    bool god;
  };

}