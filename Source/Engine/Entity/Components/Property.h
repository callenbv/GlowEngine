/*
/
// filename: Property.h
// author: Callen Betts
// brief: Defines a serializable field for exposing variables to the editor
/
*/

#pragma once
#include <variant>

using VarType = std::variant<int*, float*, bool*, std::string*, Vector3D*>;

enum class VariableType
{
	String,
	Int,
	Bool,
	Float,
	Vector
};

struct Variable
{
	std::string name;
	VarType value;
	VariableType type;

  // construct a new variable 
	Variable(std::string name_, VarType val) : name(name_), value(val)
	{
    type = determineType(val);
	}

  // create a way to display our data in ImGui
  void display();

  /// <summary>
  /// Convert a vector into a json object
  /// </summary>
  /// <param name="j"> Json object </param>
  /// <param name="v"> Vector to convert </param>
  void ToJson(nlohmann::json& j, const Vector3D& v)
  {
    j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
  }

  /// <summary>
  // Save a variable (return its value in a json serializable way)
  /// </summary>
  /// <returns> A const json object </returns>
  nlohmann::json Save()
  {
    nlohmann::json jsonData;

    // Use std::visit to handle the different types in the variant
    // Dereference the pointer to get the actual value
    std::visit([&jsonData, this](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int*>) {
          // Saving ints
          jsonData["value"] = *arg;
        }
        else if constexpr (std::is_same_v<T, float*>) {
          // Saving floats
          jsonData["value"] = *arg;
        }
        else if constexpr (std::is_same_v<T, bool*>) {
          // Saving bools
          jsonData["value"] = *arg;
        }
        else if constexpr (std::is_same_v<T, std::string*>) {
          // Saving strings
          jsonData["value"] = *arg;
        }
        else if constexpr (std::is_same_v<T, Vector3D*>) {
          // Saving vectors
          ToJson(jsonData["value"], *arg);
        }
      }, value);

    return jsonData;
  }

private:

  // used to define a type for switch statement use in inspector
  static VariableType determineType(const VarType& val)
  {
    return std::visit([](auto&& arg) -> VariableType {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, int*>)
        return VariableType::Int;
      else if constexpr (std::is_same_v<T, float*>)
        return VariableType::Float;
      else if constexpr (std::is_same_v<T, bool*>)
        return VariableType::Bool;
      else if constexpr (std::is_same_v<T, std::string*>)
        return VariableType::String;
      else if constexpr (std::is_same_v<T, Vector3D*>)
        return VariableType::Vector;
      else
        throw std::runtime_error("Unknown type");
      }, val);
  }
};

// create a variable to be modified in the editor
Variable CreateVariable(std::string name, int* val);
Variable CreateVariable(std::string name, float* val);
Variable CreateVariable(std::string name, Vector3D* val);
Variable CreateVariable(std::string name, std::string* val);
Variable CreateVariable(std::string name, bool* val);
