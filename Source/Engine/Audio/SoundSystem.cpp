/*
/
// filename: SoundSystem.cpp
// author: Callen Betts
// brief: implements SoundSystem.h
/
*/

#include "stdafx.h"
#include "Sound.h"
#include "SoundSystem.h"
#include "SoundLibrary.h"
#include "Engine/GlowEngine.h"
#include "fmod/fmod.hpp"

// setup FMOD 
Audio::SoundSystem::SoundSystem() : System("Audio System")
{
  lib = engine->getSoundLibrary();
  musicChannel = nullptr;
  soundChannel = nullptr;

  FMOD_RESULT result = FMOD::System_Create(&system);
  result = system->init(512, FMOD_INIT_NORMAL, nullptr);

  // create FMOD sound objects from the library automagically
  for (auto it = lib->getSounds().begin(); it != lib->getSounds().end();)
  {
    Sound* sound = (*it).second;

    system->createSound(sound->getFilePath().c_str(), sound->getMode(), nullptr, sound->getRawData());
    Logger::write("Loaded sound " + sound->getFilePath());
    ++it;
  }

}

Audio::SoundSystem::~SoundSystem()
{
  for (auto it = lib->getSounds().begin(); it != lib->getSounds().end(); ++it)
  {
    Sound* sound = (*it).second;
    sound->get()->release();
  }

  system->release();
}

// create a new sound object given the archetype
void Audio::SoundSystem::playSound(std::string name)
{
  // get the sound archetype
  Sound* sound = lib->get(name);

  if (!sound)
    return;

  // change the channel based on type
  switch (sound->getType())
  {
  case Audio::Type::Music:
      system->playSound(sound->get(), nullptr, false, &musicChannel);
        break;

  case Audio::Type::Sound:
      system->playSound(sound->get(), nullptr, false, &soundChannel);
      break;
  }
  musicChannel->setVolume(0.3f);
}

void Audio::SoundSystem::update()
{
  system->update();

}

void Audio::playSound(std::string name)
{
  EngineInstance::getEngine()->getSoundSystem()->playSound(name);
}
