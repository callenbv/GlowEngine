/*
/
// filename: stub.h
// author: Callen Betts
// brief: defines stub class
/
*/

#pragma once

#include "fmod/fmod.hpp"

namespace Audio
{
  // define whether or not audio is sound or music
  enum class Type
  {
    Sound,
    Music
  };

  class Sound
  {

  public:

    Sound(std::string soundName, std::string fileType, std::string path, bool shouldLoop, Audio::Type audioType = Audio::Type::Sound);
    ~Sound();
    
    // getters
    std::string getName();
    std::string getFilePath();
    std::string getFileType();
    FMOD_MODE getMode();
    Audio::Type getType();

    // retrieves the FMOD sound data
    FMOD::Sound** getRawData();
    FMOD::Sound* get();

    // set the mode
    void setMode(FMOD_MODE mode);
    void setData(FMOD::Sound* data);

  private:

    // identifiers
    std::string name;
    std::string fileType;
    std::string filePath;
    Audio::Type type;
    FMOD_MODE mode;

    bool loop = false;

    FMOD::Sound* soundData;
  };
}