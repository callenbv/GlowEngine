/*
/
// filename: Settings.cpp
// author: Callen Betts
// brief: implements Settings.h
/
*/

#include "stdafx.h"
#include "Settings.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"

void Editor::Settings::update()
{
	gameSettings.update();
}
