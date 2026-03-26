/*
/
// filename: GameSettings.h
// author: Callen Betts
// brief: defines GameSettings class
/
*/

#pragma once
#include "Engine/Graphics/UI/Editor/Widget.h"

namespace Editor
{

	class GameSettings 
	{

	public:
		
		GameSettings();

		void update();

		void OnPlay();
		void OnPause();
		void OnStop();

	private:

		Engine::GlowEngine* engine;
		Scene::SceneSystem* sceneSystem;

	};

}