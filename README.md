# GlowEngine

## Demo

Build with Visual Studio: open `GlowEngine.sln`, pick Debug | x64 or Release | x64, then Build → Build Solution (`Ctrl+Shift+B`). Pre-link runs `run.bat`, which copies `GlowEngine/Assets` into `x64/Debug/Assets/`. Run `GlowEngine.exe` from the build output: Debug builds land at `x64/Debug/GlowEngine.exe` (console subsystem for logging); Release at `x64/Release/GlowEngine.exe` (Windows subsystem). The sample scene uses the ECS, rendering, physics, editor UI, and camera movement.

Controls: WASD to move the camera, Space up, Shift down, mouse to look, right-click to pivot in-scene, Tab for fullscreen, G to toggle debug visuals.

---

A custom C++ 3D game engine for Windows, built on DirectX 11. It uses an entity–component–system (ECS) layout, a real-time forward-style rendering pipeline (including shadow mapping), integrated ImGui tooling, and gameplay-oriented systems such as physics and audio.

Rendering covers a DirectX 11 renderer, HLSL shaders (including shadow map passes), materials, mesh and texture loading (`stb_image`), lighting buffers, and a shadow system. Scenes include sample content (e.g. a forest scene) and JSON-driven entities under `GlowEngine/Data/Entities/` with Assimp-based model loading. Gameplay includes ECS-style entities, transforms, physics, box colliders, collision resolution, and player behavior. The in-engine editor (`GlowGui`) exposes a game window, scene editor, inspector, console, resources, and settings. Audio goes through FMOD; input supports keyboard, mouse, and the demo camera controls.

## Tech stack

- Language: C++17 (`stdcpp17` on x64 configurations)
- Graphics: DirectX 11, HLSL (`Source/Shaders/`)
- UI: Dear ImGui (Win32 + DX11 backends)
- Models: [Assimp](https://www.assimp.org/) (linked as `assimp-vc143-mt.lib`)
- Audio: [FMOD](https://www.fmod.com/) (linked as `fmodL_vc.lib` on x64)
- Other: JSON parsing for entities ([nlohmann/json](https://github.com/nlohmann/json) single-header in-tree), precompiled headers on main engine units

## Requirements

- OS: Windows 10 or later (project targets Windows SDK `10.0`)
- IDE: Visual Studio 2022 (toolset v143; solution format VS 2022)
- Workload: Desktop development with C++, including the Windows 10/11 SDK and MSVC toolset
- Optional: Windows SDK version matching `WindowsTargetPlatformVersion` in `GlowEngine.vcxproj` if you hit SDK-related build errors

Third-party import libraries are expected under `GlowEngine/Lib/` (e.g. Assimp and FMOD). If those folders are empty in your clone, obtain matching v143 / x64 builds and place `.lib` files (and any required runtime DLLs next to the built `.exe`) before running.

## Repository layout (high level)

```
GlowEngine.sln          # Visual Studio solution
run.bat                 # Pre-link: copies Assets → x64/Debug/Assets
GlowEngine/
  Assets/               # Meshes, textures, etc.
  Data/Entities/        # JSON entity descriptions
  Include/              # Third-party headers (e.g. Assimp, FMOD)
  Lib/                  # Third-party .lib files (populate locally if missing)
  Source/
    Engine/             # Core engine: graphics, ECS, audio, input, …
    Game/               # Game layer: scenes, behaviors, systems
    Shaders/            # HLSL shaders
```

## Credits

Callen Betts

---

Educational / portfolio engine; APIs and third-party terms apply to Assimp, FMOD, ImGui, and Microsoft SDKs.
