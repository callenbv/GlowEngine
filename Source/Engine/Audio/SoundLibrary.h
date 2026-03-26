/*
/
// filename: stub.h
// author: Callen Betts
// brief: defines stub class
/
*/

#pragma once

namespace Audio
{
  class Sound; // forward declare

  class SoundLibrary
  {
    
  public:

    SoundLibrary();

    void load();

    Sound* get(std::string name);

    const std::map<std::string, Sound*>& getSounds();

  private:

    std::map<std::string, Sound*> sounds;

  };
}