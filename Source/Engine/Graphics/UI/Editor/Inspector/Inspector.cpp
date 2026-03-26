/*
/
// filename: Inspector.cpp
// author: Callen Betts
// brief: implements Inspector.h
/
*/

#include "stdafx.h"
#include "Inspector.h"
#include "Engine/Entity/Components/Component.h"

// static varibles initialized here
Entities::Entity* Editor::Inspector::selectedEntity = nullptr;
Entities::Entity* Editor::Inspector::previousEntity = nullptr;

Editor::Inspector::Inspector(std::string title, std::string desc, ImGuiWindowFlags flags): Widget(title, desc, flags)
{

}

void Editor::Inspector::update()
{
	// deselect if not focus
	if (EngineInstance::IsPlaying() && !EngineInstance::IsPaused())
		inspect(nullptr);

	// if we have a selected entity, we allow changing its properties
	if (selectedEntity)
	{
		selectedEntity->SetSelected(true);

		// move the entity with arrow keys
		Components::Transform* transform = getComponentOfType(Transform, selectedEntity);

		float adjustSpeed = 0.1f;

		if (Input::InputSystem::KeyDown(VK_LEFT))
			transform->setPosition(transform->getPosition() + Vector3D(-adjustSpeed,0,0));
		if (Input::InputSystem::KeyDown(VK_RIGHT))
			transform->setPosition(transform->getPosition() + Vector3D(adjustSpeed, 0, 0));
		if (Input::InputSystem::KeyDown(VK_UP))
			transform->setPosition(transform->getPosition() + Vector3D(0, 0, -adjustSpeed));
		if (Input::InputSystem::KeyDown(VK_DOWN))
			transform->setPosition(transform->getPosition() + Vector3D(0, 0, adjustSpeed));
		if (Input::InputSystem::KeyDown('Z'))
			transform->setPosition(transform->getPosition() + Vector3D(0, adjustSpeed, 0));
		if (Input::InputSystem::KeyDown('X'))
			transform->setPosition(transform->getPosition() + Vector3D(0, -adjustSpeed, 0));

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// for each variable type, allow modification; we can only edit variable types explicitly defined
		for (auto& variable : selectedEntity->getVariables())
		{
			variable.display();
		}

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// popup window for adding and removing components
		if (openContextMenu && ImGui::BeginPopupContextWindow("COMPONENTS"))
		{

			if (ImGui::MenuItem("Delete Component"))
			{
				// delete the selected component
				Components::Component* component = (Components::Component*) selectedObject;
				selectedEntity->DeleteComponent(component);
				selectedObject = nullptr;
			}

			if (ImGui::MenuItem("Add Component"))
			{
				// delete the selected component
				Components::Component* component = (Components::Component*)selectedObject;
				selectedEntity->DeleteComponent(component);
				selectedObject = nullptr;
			}

			ImGui::EndPopup();
		}

		// iterate over every component's properties; each serializable property will be modifiable
		// this is done using a vector of properties in each component
		for (const auto& component : selectedEntity->getComponents())
		{
			std::string componentName = component->getName();

			bool open = ImGui::TreeNode(componentName.c_str());

			// select to delete
			if (ImGui::IsItemClicked(1))
			{
				openContextMenu = !openContextMenu;
				selectedObject = component;
			}

			if (open) // begin component tree node *
			{
				ImGui::NewLine();

				// some components have specific custom displays
				component->display();

				for (auto& variable : component->getVariables())
				{
					// label the variable; some variables might have custom displays, we will add this layer
					variable.display();
				}

				ImGui::NewLine();
				ImGui::TreePop(); // end component tree node *
			}
		}

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		// add a new component if we want
		bool componentList = ImGui::TreeNode("Add New Component");

		if (componentList)
		{
			for (const auto& type : ComponentFactory::instance().getRegisteredTypes())
			{
				// get the type of component
				std::string name = type.c_str();

				// check if we have the component
				if (!selectedEntity->hasComponent(type)) 
				{
					if (ImGui::Selectable(type.c_str())) 
					{
						selectedEntity->addComponent(type);
					}
				}
			}
			ImGui::TreePop();
		}

	}
}

// called while we are dragging an entity or have one selected
void Editor::Inspector::DragEntity()
{
	//Components::Transform* transform = getComponentOfType(Transform, selectedEntity);
	//transform->recalculateMatrix();

	//float pos[3] = {transform->getPosition().x,transform->getPosition().y,transform->getPosition().z};
	//if (ImGui::DragFloat3("Position", pos))
	//{
	//	transform->setPosition({ pos[0],pos[1],pos[2] });
	//}
}

// when we inspect an entity, select it and track previous and current
void Editor::Inspector::inspect(Entities::Entity* ent)
{
	previousEntity = selectedEntity;
	selectedEntity = ent;

	if (previousEntity)
	{
		previousEntity->SetSelected(false);
	}
	if (previousEntity && !ent)
	{
		previousEntity->SetSelected(false);
	}
}
