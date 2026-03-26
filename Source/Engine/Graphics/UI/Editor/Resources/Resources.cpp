/*
/
// filename: stub.cpp
// author: Callen Betts
// brief: implements stub.h
/
*/

#include "stdafx.h"
#include "Resources.h"
#include "Engine/GlowEngine.h"
#include "Engine/Entity/EntityFactory.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"

void Editor::Resources::update()
{
	Entities::EntityFactory* factory = EngineInstance::getEngine()->getEntityFactory();
	Textures::TextureLibrary* lib = EngineInstance::getEngine()->getTextureLibrary();

	for (auto& entry : factory->GetArchetypes())
	{
		Entities::Entity* archetype = entry.second;
		std::string fileName = entry.first;

		// for each archetype, we want to be able to display their name and drag them into our scene
		// we want to be able to drag them into the hierarchy and the game window
		if (archetype)
		{
			// get the shader resoruce view associated with the entity so we can display it when dragging
			ID3D11ShaderResourceView* fileIcon = *lib->get("File")->getTextureView();
			ImGui::BeginGroup();
			ImGui::Image((void*)fileIcon, { 48, 48 });
			ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 100.f);
			ImGui::TextUnformatted(fileName.c_str());
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 75.f);
			ImGui::EndGroup();
			ImGui::SameLine();

			// drag the object into the scene
		}
	}

}
