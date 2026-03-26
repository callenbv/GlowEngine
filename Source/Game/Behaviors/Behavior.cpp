/*
/
// filename: Behavior.cpp
// author: Callen Betts
// brief: implements Behavior.h
/
*/

#include "stdafx.h"
#include "Behavior.h"
#include "Engine/EngineInstance.h"
#include "Engine/GlowEngine.h"

// create a behavior
Game::Behavior::Behavior() : Component()
{
  type = Components::Component::ComponentType::Behavior;
  engine = EngineInstance::getEngine();
  input = engine->getInputSystem();
  simulation = false;
}

