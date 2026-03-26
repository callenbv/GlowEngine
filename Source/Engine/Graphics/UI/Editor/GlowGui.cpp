/*
/
// filename: GlowGui.cpp
// author: Callen Betts
// brief: implements GlowGui.h
/
*/

#include "stdafx.h"
#include "GlowGui.h"
#include "Engine/EngineInstance.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/Window/Window.h"
#include "Engine/Graphics/UI/Editor/Widget.h"
#include "Engine/Graphics/UI/Editor/Inspector/Inspector.h"
#include "Engine/Graphics/UI/Editor/SceneEditor/SceneEditor.h"
#include "Engine/Graphics/UI/Editor/Settings/Settings.h"
#include "Engine/Graphics/UI/Editor/Console/Console.h"
#include "Engine/Graphics/UI/Editor/Resources/Resources.h"
#include "Engine/Graphics/UI/Editor/Inspector/EngineInspector.h"
#include "Engine/Graphics/UI/Editor/GameWindow/GameWindow.h"

Graphics::GlowGui::GlowGui(HWND windowHandle, ID3D11Device* device, ID3D11DeviceContext* context, Graphics::Renderer* renderer)
  :
  windowHandle(windowHandle),
  device(device),
  context(context),
  renderer(renderer)
{
  // initialize the ImGui system
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplWin32_Init(windowHandle);
  ImGui_ImplDX11_Init(device, context);

  ImGui::StyleColorsDark();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.Fonts->AddFontFromFileTTF("Assets/Fonts/CascadiaCode.ttf", 16);

  // create game window first
  gameWindow = new Editor::GameWindow("Otherglow");
  // create inspector
  Editor::Inspector* inspector = new Editor::Inspector("Inspector");
  widgets.push_back(inspector);
  // engine inspector
  Editor::EngineInspector* engineInspector = new Editor::EngineInspector("Engine");
  widgets.push_back(engineInspector);
  // create settings
  Editor::Settings* settings = new Editor::Settings("Settings");
  widgets.push_back(settings);
  // create console window
  Editor::Console* console = new Editor::Console("Console");
  widgets.push_back(console);
  // create resources window
  Editor::Resources* resources = new Editor::Resources("Resources");
  widgets.push_back(resources);
  // create scene editor
  Editor::SceneEditor* editor = new Editor::SceneEditor("Scene Editor");
  widgets.push_back(editor);
}

void Graphics::GlowGui::beginUpdate()
{
  // initialize a new ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // define the flags for docking space
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImVec2 size = { (float)renderer->getWindow()->getWidth(), (float)renderer->getWindow()->getHeight() };
  // define the main viewport of ImGui
  ImGuiViewport* viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(size);
  ImGui::SetNextWindowViewport(viewport->ID);


  // create docking space
  ImGui::Begin("DockSpace", nullptr, window_flags);
  ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(dockspace_id);
  ImGui::End();
}

// anything called between the begin and end update is rendered
// here, we will invoke any draw calls
void Graphics::GlowGui::update()
{
  // beginning updates
  beginUpdate();
  
  // update our game window
  gameWindow->renderFrame();

  // render each of our ImGui widgets
  for (auto& widget : widgets)
  {
    widget->renderFrame();
  }
}

// draw the ImGui render data and end the frame
void Graphics::GlowGui::endUpdate()
{
  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// clean up ImGui resources
void Graphics::GlowGui::cleanUp()
{
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}
