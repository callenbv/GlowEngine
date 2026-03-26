/*
/
// filename: stub.h
// author: Callen Betts
// brief: defines stub class
/
*/

#pragma once
#include "Engine/Graphics/UI/Editor/Widget.h"

namespace Editor
{

	class Resources : public Widget
	{

	public:

		Resources(std::string title, std::string desc = "", ImGuiWindowFlags flags = 0) : Widget(title, desc, flags) {};

		void update();

	};

}