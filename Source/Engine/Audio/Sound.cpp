/*
/
// filename: Sound.cpp
// author: Callen Betts
// brief: implements Sound.h
/
*/

#include "stdafx.h"
#include "Sound.h"

Audio::Sound::Sound(std::string soundName, std::string fileExtension,  std::string path, bool shouldLoop, Audio::Type audioType)
  :
  name(soundName),
  fileType(fileExtension),
  filePath(path),
  mode(FMOD_DEFAULT),
  loop(shouldLoop),
  type(audioType)
{
  // add a loopable mode
  if (loop)
  {
    mode |= FMOD_LOOP_NORMAL;
  }
}

// free sound data
Audio::Sound::~Sound()
{

}

std::string Audio::Sound::getName()
{
  return name;
}

std::string Audio::Sound::getFilePath()
{
  return filePath;
}

std::string Audio::Sound::getFileType()
{
  return fileType;
}

FMOD_MODE Audio::Sound::getMode()
{
  return mode;
}

Audio::Type Audio::Sound::getType()
{
  return type;
}

FMOD::Sound** Audio::Sound::getRawData()
{
  return &soundData;
}

FMOD::Sound* Audio::Sound::get()
{
  return soundData;
}

void Audio::Sound::setMode(FMOD_MODE newMode)
{
  mode = newMode;
}

void Audio::Sound::setData(FMOD::Sound* data)
{
  soundData = data;
}
