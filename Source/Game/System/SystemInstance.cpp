/*
/
// filename: SystemInstance.cpp
// author: Callen Betts
// brief: implements SystemInstance.h
/
*/

#include "stdafx.h"
#include "SystemInstance.h"

// define the systems vector here
// this way we can access it from the base system class
std::vector<Systems::System*> SystemInstance::systems;

// add a system to the system list
void SystemInstance::addSystem(Systems::System* system)
{
  systems.push_back(system);
}

