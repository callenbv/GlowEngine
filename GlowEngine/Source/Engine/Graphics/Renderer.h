/*
/
// filename: Rederer.h
// author: Callen Betts
// brief: defines core rendering engine
/
*/

#pragma once

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "Buffers/ConstantBuffer.h"
#include "Buffers/Buffer.h"
#include "Camera/Camera.h"
#include "Lighting/Shadows/ShadowSystem.h"
#include "Shaders/ShaderManager.h"
#include "UI/Editor/GlowGui.h"
#include "Materials/Material.h"

namespace Graphics
{
  class Renderer
  {

  public:
    Renderer(HWND handle);

    void initGraphics();
    void cleanup(); // release all of the d3d objects

    // frame updates
    void beginFrame();
    void endFrame();
    void update();

    void createDeviceAndSwapChain();
    void loadShaders();
    void createTargetView();
    void createViewport();
    void createBlendState();
    void createDepthStencil();
    void createSamplerState();

    // materials
    void BindMaterial(Materials::Material* mat);

    // update hotkeys
    void UpdateHotkeys();

    // add a buffer to the list to track
    void addBuffer(Buffer*);
    void UpdateBuffers();

    // the constant buffer needs to be updated whenever a model is being rendered
    void updateObjectBuffer();
    // update the transform matrix within the constant buffer
    void updateObjectBufferWorldMatrix(Matrix world);
    // update the perspective and camera view matrix
    void updateObjectBufferCameraMatrices();

    Graphics::Window* getWindow() { return window;}

    // helpful drawing functions 
    // set the target render color
    void drawSetColor(const Color& color);
    // set the outline color, if any
    void DrawSetOutline(const Color& color);

    // toggle fullscreen
    void toggleFullscreen();
    bool isFullscreen() { return fullscreen; }

    // unbind a texture 
    void unBindTexture();

    // rasterizer states and helpers
    void createRasterizer();
    void setRasterizerFillMode(D3D11_FILL_MODE fillMode = D3D11_FILL_WIREFRAME);

    // clear the target view with a background colour
    void clearTargetView();
    void setRenderTargetProperties(float x, float y, float width, float height);
    void setBackgroundColor(float color[4]);

    void toggleDebugMode();
    bool isDebugMode();
    bool& getDebugMode() { return debug; }

    // set the topology with a default of trianglelist
    void setTopology(D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // render our game to a texture
    void SetRenderTarget();
    void SetDefaultRenderTarget();

    // manage point lights
    void addPointLight(PointLight* light);
    void updatePointLight(PointLight* light);

    // get the directX devices for draw calls
    ID3D11Device* getDevice();
    ID3D11DeviceContext* getDeviceContext();

    // create the ImGui system
    void createImGuiSystem();

    // get the back buffer
    ID3D11Texture2D* getBackBuffer();
    ID3D11ShaderResourceView* GetGameTexture();

    // get the camera
    Visual::Camera* getCamera() { return camera; }
    Lighting::ShadowSystem* GetShadowSystem() { return shadowSystem; }

    // set draw data for each object (per object buffer)
    // set the UV scale coords
    void SetUVScale(float x, float y);

  private:

    // shadow system
    Lighting::ShadowSystem* shadowSystem;

    // render targets 
    ID3D11ShaderResourceView* backBufferSRV;
    D3D11_TEXTURE2D_DESC textureDesc;

    // window handle
    HWND windowHandle;

    // ImGui system
    Graphics::GlowGui* glowGui;

    // for rendering scene to a texture
    ID3D11Texture2D* renderTargetTexture = nullptr;
    ID3D11RenderTargetView* renderTargetViewGui = nullptr;
    ID3D11ShaderResourceView* shaderResourceView = nullptr;
    ID3D11SamplerState* shadowSampler = nullptr;

    // devices
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;

    // target view and texture
    ID3D11RenderTargetView* renderTargetView;

    // render target view
    ID3D11Texture2D* backBuffer;

    // rasterizer
    ID3D11RasterizerState* wireframeRasterizerState;
    D3D11_RASTERIZER_DESC rasterizerDesc;

    // sampler
    ID3D11SamplerState* sampler;
    ID3D11ShaderResourceView* shadowShaderView;

    // blend state
    ID3D11BlendState* blendState;

    // depth stencil
    ID3D11DepthStencilView* depthStencilView;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11DepthStencilState* depthStencilState;

    // shaders
    ID3D11PixelShader* pixelShader;
    ID3D11VertexShader* vertexShader;

    // buffers
    ConstantBuffer<ColorBuffer>* colorBuffer;
    ConstantBuffer<ColorBuffer>* outlineBuffer;
    ConstantBuffer<cbPerObject>* objectBuffer;
    ConstantBuffer<PointLightBuffer>* lightBuffer;
    ConstantBuffer<GlobalLightBuffer>* globalLightBuffer;
    ConstantBuffer<DrawDataBuffer>* drawDataBuffer;
    ConstantBuffer<Materials::MaterialBufferCPU>* materialBuffer;

    float shadowMapWidth = 1024;
    float shadowMapHeight = 1024;

    // we hold a vector of constant buffers that are globally updated and bound
    // you can decide to disable this when creating the buffer
    std::vector<Buffer*> buffers;

    // background color
    float backgroundColor[4];

    // engine
    Engine::GlowEngine* engine;
    // camera
    Visual::Camera* camera;
    // window
    Graphics::Window* window;
    // shader manager
    Shaders::ShaderManager* shaderManager;

    // point lights - the renderer can have a variable amount of point lights
    int lights;
    bool fullscreen = false;
    bool debug = false;

  };

} // graphics