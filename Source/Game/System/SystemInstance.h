/*
/
// filename: SystemInstance.h
// author: Callen Betts
// brief: defines SystemInstance class
/
*/

#pragma once

// define an instance of the system list so we can access and create systems in
// the base class
class SystemInstance
{

public:

  // add a system to the vector - system base class constructor does this automatically
  static void addSystem(Systems::System* system);

  // get the systems vector
  static std::vector<Systems::System*> getSystems() { return systems; }

private:

  // engine contains a vector of systems
  static std::vector<Systems::System*> systems;

};
