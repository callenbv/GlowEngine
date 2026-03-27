# GlowEngine

A custom 3D game engine built in C++ with DirectX 11, focused on real-time rendering, engine architecture, and gameplay systems.

---

## Demo

1. Open the `Demo/Release` folder
2. Run `GlowEngine.exe`  
3. Explore the scene by pressing the "Play" button icon 

Controls:  
WASD — Move  
Mouse — Look  
Space / Shift — Up / Down  
Right Click — Pivot  
Tab — Fullscreen  
G — Debug visuals  
ESCAPE - Stop Game/Force Quit

---

## Overview

GlowEngine is a personal project designed to explore how modern game engines are built from the ground up.

It includes a real-time rendering pipeline, an ECS-based architecture, and a custom in-engine editor for creating and inspecting scenes.

---

## Key Features

### Rendering
- DirectX 11 renderer with shadow mapping  
- HLSL shaders and lighting system  
- Model and texture loading (Assimp, stb_image)  

### Engine Architecture
- Entity–Component–System (ECS) design  
- JSON-driven entities and scenes  

### Editor Tools
- Built-in editor (ImGui-based) with:
  - Scene view  
  - Inspector  
  - Console  
  - Resource panel  

### Gameplay Systems
- Transform and physics system  
- Collision detection and resolution  
- Player controller  

### Audio
- FMOD integration for real-time audio  

---

## Tech Stack

- C++17  
- DirectX 11 and HLSL  
- Dear ImGui  
- Assimp  
- FMOD  

---

## Purpose

This project was built to deepen understanding of:
- Real-time rendering pipelines  
- Game engine architecture (ECS)  
- Tooling and editor workflows  

---

## Author

Callen Betts  

---
