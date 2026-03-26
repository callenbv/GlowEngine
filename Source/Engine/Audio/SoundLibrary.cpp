/*
/
// filename: SoundLibrary.cpp
// author: Callen Betts
// brief: implements SoundLibrary.h
/
*/

#include "stdafx.h"
#include "SoundLibrary.h"
#include "Sound.h"
#include <filesystem>

namespace fs = std::filesystem;

Audio::SoundLibrary::SoundLibrary()
{
  load();
}

void Audio::SoundLibrary::load()
{
  std::string directoryPath = "Assets/Audio";

  if (fs::is_directory(directoryPath) && fs::exists(directoryPath))
  {
    for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
    {
      if (fs::exists(entry) && fs::is_regular_file(entry))
      {
        std::string fileName = entry.path().stem().string();
        std::string filePath = entry.path().string();
        std::string fileType = entry.path().extension().string();

        // files must be of type ogg or mp3
        if (fileType == ".ogg" || fileType == ".mp3" || fileType == ".wav")
        {
          Audio::Type type = Audio::Type::Sound;
          bool loop = false;
          fs::path pathObj(filePath);
          std::string directory = pathObj.parent_path().stem().string();

          if (directory == "Music")
          {
            type = Audio::Type::Music;
            loop = true;
          }

          sounds[fileName] = new Sound(fileName, fileType, filePath, loop, type);
        }
      }
    }
  }
}

Audio::Sound* Audio::SoundLibrary::get(std::string name)
{
  return sounds[name];
}

const std::map<std::string, Audio::Sound*>& Audio::SoundLibrary::getSounds()
{
  return sounds;
}
