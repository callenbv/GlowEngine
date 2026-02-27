/*
/
// filename: stdafx.h
// author: Callen Betts
// brief: precompiled header
/
*/

#pragma once

// windows
#include "Windows/targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include "Windows/Resource.h"

// directX math
#include <DirectXMath.h>

// standard includes
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>

// json & deserialization
#include "Engine/Systems/Parsing/json.hpp"

// engine
#include "Engine/EngineInstance.h"
#include "Engine/Global.h"

// math
#include "Engine/Math/GlowMath.h"
#include "Engine/Math/Vertex.h"
#include "Engine/Math/Random.h"

using namespace GlowMath;

// systems
#include "Engine/Systems/Logger/Log.h"
#include "Engine/Systems/Input/Input.h"
#include "Game/System/System.h"
#include "Game/System/SystemInstance.h"
#include "Engine/Audio/SoundSystem.h"

// components
#include "Engine/Entity/Components/Visual/Models/Model.h"
#include "Engine/Entity/Components/Physical/Transform.h"
#include "Engine/Entity/Components/Visual/Sprite3D.h"
#include "Engine/Entity/Components/Component.h"
#include "Engine/Entity/Components/Physical/Physics.h"
#include "Engine/Entity/Components/Collision/Collider.h"
#include "Engine/Entity/Components/Collision/BoxCollider.h"
#include "Engine/Entity/Components/Collision/BoundingBox.h"
#include "Engine/Entity/Actor.h"
#include "Engine/Entity/Components/Property.h"

// ImGui
#include "Include/ImGui/imgui.h"
#include "Include/ImGui/imgui_impl_win32.h"
#include "Include/ImGui/imgui_impl_dx11.h"

// graphics
#include "Engine/Graphics/Color/Color.h"

// macros
#define NOMINMAX

