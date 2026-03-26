/*
/
// filename: SoundSystem.h
// author: Callen Betts
// brief: defines SoundSystem class
/
*/

#pragma once

namespace FMOD
{
  class System;
  class Channel;
}

namespace Audio
{

  class SoundLibrary; // forward declare library

  class SoundSystem : public Systems::System
  {

  public:

    SoundSystem();
    ~SoundSystem();

    void playSound(std::string name);
    void update();

  private:

    SoundLibrary* lib;

    // audio channels and system
    FMOD::System* system;
    FMOD::Channel* soundChannel;
    FMOD::Channel* musicChannel;
  };

  void playSound(std::string name);
}