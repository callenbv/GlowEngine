/*
/
// filename: Property.cpp
// author: Callen Betts
// brief: Creates a variable property that can be serialized via binary or JSON
/
*/

#include "stdafx.h"
#include "Property.h"

Variable CreateVariable(std::string name, int* val)
{
  return Variable(name, val);
}

Variable CreateVariable(std::string name, float* val)
{
  return Variable(name, val);
}

Variable CreateVariable(std::string name, Vector3D* val)
{
  return Variable(name, val);
}

Variable CreateVariable(std::string name, bool* val)
{
  return Variable(name, val);
}

Variable CreateVariable(std::string name, std::string* val)
{
  return Variable(name, val);
}

// depending on the type, we give different modification options
// this is done through here by default, but for specific uses the user doesn't need to call this
void Variable::display()
{
	ImGui::Text(name.c_str());

	switch (type)
	{
	case VariableType::String:
		ImGui::InputText(("##" + name).c_str(), std::get<std::string*>(value)->data(), 16);
		break;
	case VariableType::Bool:
		ImGui::Checkbox(("##" + name).c_str(), std::get<bool*>(value));
		break;
	case VariableType::Int:
		ImGui::InputInt(("##" + name).c_str(), std::get<int*>(value));
		break;
	case VariableType::Float:
		ImGui::InputFloat(("##" + name).c_str(), std::get<float*>(value));
		break;
	case VariableType::Vector:
		Vector3D* vec = std::get<Vector3D*>(value);
		ImGui::InputFloat(("##" + name + "x").c_str(), &vec->x);
		ImGui::InputFloat(("##" + name + "y").c_str(), &vec->y);
		ImGui::InputFloat(("##" + name + "z").c_str(), &vec->z);
		break;
	}
}
