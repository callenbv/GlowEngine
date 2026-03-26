/*
/
// filename: main.cpp
// author: Callen Betts
// brief: implements main game loop
/
*/

#include "stdafx.h"
#include "Engine/GlowEngine.h"

// create engine
static Engine::GlowEngine* engine = new Engine::GlowEngine();

int main()
{
  // start the engine
  if (engine->start())
  {
    return engine->run();
  }

  return -1; // catastrophic engine failiure
}
