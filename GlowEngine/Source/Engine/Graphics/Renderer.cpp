/*
/
// filename: Renderer.cpp
// author: Callen Betts
// brief: implements the rendering engine and pipeline
/
*/

#include "stdafx.h"
#include "Renderer.h"
#include "Engine/GlowEngine.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityList/EntityList.h"
#include "Engine/Graphics/Textures/Texture.h"
#include "Engine/Graphics/Window/Window.h"
#include "Engine/Graphics/Textures/stb_image.h"
#include "Engine/Graphics/Textures/TextureLibrary.h"
#include <filesystem>
#include "Game/Scene/SceneSystem.h"

// define the amount of max lights we can have
#define MAXLIGHTS 8
static PointLight* pointLightsArray[MAXLIGHTS];

// initialize the graphics renderer properties
Graphics::Renderer::Renderer(HWND handle)
  :
  windowHandle(handle),
  pixelShader(nullptr),
  vertexShader(nullptr),
  camera(nullptr),
  sampler(nullptr),
  glowGui(nullptr),
  lights(0)
{
  // engine
  engine = EngineInstance::getEngine();
  // window
  window = engine->getWindow();
  // graphics
  initGraphics();
  // camera
  camera = new Visual::Camera(this);
  // constant buffers
  buffers.push_back(objectBuffer = new ConstantBuffer<cbPerObject>(device, deviceContext, 0, false, ShaderType::Vertex));
  buffers.push_back(drawDataBuffer = new ConstantBuffer<DrawDataBuffer>(device, deviceContext, 0, false, ShaderType::Pixel));
  buffers.push_back(lightBuffer = new ConstantBuffer<PointLightBuffer>(device, deviceContext, 0, false, ShaderType::Pixel));
  buffers.push_back(globalLightBuffer = new ConstantBuffer<GlobalLightBuffer>(device, deviceContext, 1, true, ShaderType::Pixel));
  buffers.push_back(colorBuffer = new ConstantBuffer<ColorBuffer>(device, deviceContext, 2, false, ShaderType::Pixel));
  buffers.push_back(outlineBuffer = new ConstantBuffer<ColorBuffer>(device, deviceContext, 3, false, ShaderType::Pixel));
  // shadow system
  shadowSystem = new Lighting::ShadowSystem(device,deviceContext);
  //background
  float bgCol[4] = { 0.4f,0.3f,0.4f,1.f };
  setBackgroundColor(bgCol);
}

// initialize each part of the graphics pipeline
void Graphics::Renderer::initGraphics()
{
  // create each component of the pipeline starting device and swap chain
  createDeviceAndSwapChain();
  createDepthStencil();
  createTargetView();
  loadShaders();
  createBlendState();
  createViewport();
  createRasterizer();
  setTopology();
  createSamplerState();
  createImGuiSystem();
}

// free all directX objects from memory
void Graphics::Renderer::cleanup()
{
  // remove all buffers from memory
  for (auto& buffer : buffers)
  {
    delete buffer;
  }
}

// the beginning of each frame of the render engine
void Graphics::Renderer::beginFrame()
{
  // Set our render target to draw to a texture that isn't the back buffer
  SetRenderTarget();

  // update the camera matrix
  camera->update();

  // update our buffer data
  UpdateBuffers();

  // bind the texture sampler
  deviceContext->PSSetSamplers(0, 1, &sampler);

  // Update hotkeys (for toggling grahpics)
  UpdateHotkeys();

  // update the GUI widgets
  glowGui->update();
}

void Graphics::Renderer::update()
{

}

// the end of each frame at the renderer engine
// present the swapchain and draw the objects
void Graphics::Renderer::endFrame()
{
  // set the default render target of the backbuffer
  SetDefaultRenderTarget();

  // end imgui updates
  glowGui->endUpdate();

  // present the back buffer to the screen
  swapChain->Present(1, 0);
}

// initialize the d3d device, context and swap chain
void Graphics::Renderer::createDeviceAndSwapChain()
{
  // Describe our SwapChain
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

  swapChainDesc.BufferCount = 2;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
  swapChainDesc.OutputWindow = windowHandle;
  swapChainDesc.Windowed = TRUE;
  swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  swapChainDesc.BufferDesc.RefreshRate.Numerator = 144;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

  // Create a device, device context, and swap chain using the information in the swapChainDesc structure
  D3D_FEATURE_LEVEL featureLevel;
  HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
    0,
    NULL,
    0,
    D3D11_SDK_VERSION,
    &swapChainDesc,
    &swapChain,
    &device,
    &featureLevel,
    &deviceContext
  );

  // failed to create
  if (FAILED(hr))
  {
    Logger::error("Failed to create swap chain and device");
  }
}

// load compiled shader files and create the needed shaders
// there are three main shaders: pixel, pixelTex, vertex
void Graphics::Renderer::loadShaders()
{
  shaderManager = new Shaders::ShaderManager(device, deviceContext);

  // load shaders into our game (shaders are defined in there)
  shaderManager->load();

  // bind our main shaders to the device context
  vertexShader = shaderManager->getVertexShader("VertexShader");
  pixelShader = shaderManager->getPixelShader("PixelShader");

  deviceContext->VSSetShader(vertexShader, nullptr, 0);
  deviceContext->PSSetShader(pixelShader, nullptr, 0);

  // Create the input layout
  shaderManager->setup();
}

// create the target view fo the renderer
void Graphics::Renderer::createTargetView()
{
  // Get the back buffer from the swap chain
  ID3D11Texture2D* backBuffer = nullptr;
  HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
  if (FAILED(hr)) {
    // Handle error
    return;
  }

  // Create a render target view for our back buffer - this is the default one
  hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
  if (FAILED(hr)) {
    // Handle error
    backBuffer->Release();
    return;
  }

  // create a texture to render to a separate texture than the back buffer
  D3D11_TEXTURE2D_DESC textureDesc = {};
  textureDesc.Width = window->getWidth();
  textureDesc.Height = window->getHeight();
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 1;
  textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

  hr = device->CreateTexture2D(&textureDesc, nullptr, &renderTargetTexture);
  if (FAILED(hr))
  {
    // Handle error
    return;
  }

  // Describe the render target view
  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
  rtvDesc.Format = textureDesc.Format;
  rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

  hr = device->CreateRenderTargetView(renderTargetTexture, &rtvDesc, &renderTargetViewGui);
  if (FAILED(hr))
  {
    // Handle error
    renderTargetTexture->Release();
    return;
  }

  // Describe the shader resource view
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = textureDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MostDetailedMip = 0;
  srvDesc.Texture2D.MipLevels = 1;

  hr = device->CreateShaderResourceView(renderTargetTexture, &srvDesc, &shaderResourceView);
  if (FAILED(hr))
  {
    // Handle error
    renderTargetViewGui->Release();
    renderTargetTexture->Release();
    return;
  }

  backBuffer->Release();
}

// create the viewport for the renderer with given width and height
// this can be changed through the engine
void Graphics::Renderer::createViewport()
{
  // Create and set the viewport
  D3D11_VIEWPORT viewport;
  viewport.Width = static_cast<float>(window->getWidth());
  viewport.Height = static_cast<float>(window->getHeight());
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;
  deviceContext->RSSetViewports(1, &viewport);
}

// create a blend state so we can have alpha
void Graphics::Renderer::createBlendState()
{
  D3D11_BLEND_DESC blendStateDesc;
  ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
  blendStateDesc.AlphaToCoverageEnable = FALSE;
  blendStateDesc.IndependentBlendEnable = FALSE;
  blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
  blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
  blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
  blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  ID3D11BlendState* pBlendState = NULL;
  device->CreateBlendState(&blendStateDesc, &pBlendState);
  deviceContext->OMSetBlendState(pBlendState, nullptr, 0xffffffff);
}

// create a depth stencil to define stencil tests and depth ordering
void Graphics::Renderer::createDepthStencil()
{
  D3D11_TEXTURE2D_DESC depthStencilDesc;

  depthStencilDesc.Width = window->getWidth(); // Your render target width
  depthStencilDesc.Height = window->getHeight(); // Your render target height
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = 1; // Change if using MSAA
  depthStencilDesc.SampleDesc.Quality = 0; // Change if using MSAA
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  HRESULT hr = device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
  if (FAILED(hr))
  {
      throw std::exception("ERROR: Failed to create depth stencil texture");
  }

  if (!depthStencilBuffer)
  {
      throw std::exception("ERROR: Failed to create depth stencil buffer");
  }

  // Create the depth stencil view
  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
  depthStencilViewDesc.Format = depthStencilDesc.Format;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);

  if (FAILED(hr))
  {
    Logger::error("Failed to create depth stencil view");
  }

}

// create a sampler state for sampling textures
// the address mode should be wrap by default
void Graphics::Renderer::createSamplerState()
{
  D3D11_SAMPLER_DESC sampDesc;
  ZeroMemory(&sampDesc, sizeof(sampDesc));
  sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampDesc.MipLODBias = 0.0f;
  sampDesc.MaxAnisotropy = 1;
  sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
  device->CreateSamplerState(&sampDesc, &sampler);
}

/// <summary>
/// Hotkeys for toggling graphics debugging
/// </summary>
void Graphics::Renderer::UpdateHotkeys()
{
  // toggle renderer debug mode
  if (engine->getInputSystem()->keyTriggered('Q'))
  {
    toggleDebugMode();
  }
}

// renders our game to a texture 
void Graphics::Renderer::SetRenderTarget()
{
  // clear the depth stencil
  deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  // Set the render target
  deviceContext->OMSetRenderTargets(1, &renderTargetViewGui, depthStencilView);

  // Clear the render target
  deviceContext->ClearRenderTargetView(renderTargetViewGui, backgroundColor);
}

// set the render target
void Graphics::Renderer::SetDefaultRenderTarget()
{
  // clear depth buffer
  deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  // set render target
  deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void Graphics::Renderer::addBuffer(Buffer* buffer)
{
  buffers.push_back(buffer);
}

// updates all of our automatically bound buffers and some manual ones for testing
void Graphics::Renderer::UpdateBuffers()
{
  // temporary global light data for testing
  GlobalLightBuffer lightData;
  lightData.cameraPosition = { DirectX::XMVectorGetX(camera->getPosition()),DirectX::XMVectorGetY(camera->getPosition()),DirectX::XMVectorGetZ(camera->getPosition()) };
  lightData.lightColor = { 0.75f,0.75f,0.75f };
  lightData.lightDirection = { 0.5f,-0.8f,-0.5f };
  globalLightBuffer->set(lightData);

  // for now, we just have one light
  // we will update this once we have a proper lighting solution
  if (*pointLightsArray)
  {
    lightBuffer->set(pointLightsArray[0]->pointLight);
    lightBuffer->updateAndBind();
  }

  // globally applied buffers/shaders
  for (auto& buffer : buffers)
  {
    if (buffer->isGlobal())
    {
      buffer->updateAndBind();
    }
  }
}

// bind the subresource and constant buffer to the renderer
void Graphics::Renderer::updateObjectBuffer()
{
  objectBuffer->updateAndBind();
}

// given the transform matrix, update the cb object's world matrix
void Graphics::Renderer::updateObjectBufferWorldMatrix(Matrix transformMatrix)
{
  objectBuffer->get().world = DirectX::XMMatrixTranspose(transformMatrix);
}

// update the view and perspective matrices in the constant buffer using camera
void Graphics::Renderer::updateObjectBufferCameraMatrices()
{
  objectBuffer->get().view = DirectX::XMMatrixTranspose(camera->getViewMatrix());
  objectBuffer->get().projection = DirectX::XMMatrixTranspose(camera->getPerspecitveMatrix());
}

void Graphics::Renderer::drawSetColor(const Color& color)
{
  ColorBuffer colorData = { color.r, color.g, color.b, color.a };
  colorBuffer->set(colorData);
  colorBuffer->updateAndBind();
}

// sets the color we want to outline an object
void Graphics::Renderer::DrawSetOutline(const Color& color)
{
  ColorBuffer outlineData = { color.r, color.g, color.b, color.a };
  outlineBuffer->set(outlineData);
  outlineBuffer->updateAndBind();
}

void Graphics::Renderer::toggleFullscreen()
{
  fullscreen = !fullscreen;
  window->setFullscreen(fullscreen);
}

// set the texture resource to nullptr which means no texture
void Graphics::Renderer::unBindTexture()
{
  ID3D11ShaderResourceView* nullSRV[] = { nullptr };
  deviceContext->PSSetShaderResources(0, 1, nullSRV);
}

// create the rasterizer state for defining culling and vertex winding order
// you can also change this to wireframe to see the vertices connect
void Graphics::Renderer::createRasterizer()
{
  // define the rasterizer state description
  ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

  rasterizerDesc.FillMode = D3D11_FILL_SOLID; // Set the fill mode to wireframe
  rasterizerDesc.CullMode = D3D11_CULL_NONE;      // No culling to see all the lines
  rasterizerDesc.FrontCounterClockwise = false;   // Define the front-facing side
  rasterizerDesc.DepthClipEnable = true;          // Enable depth clipping

  // create the rasterizer state
  HRESULT result = device->CreateRasterizerState(&rasterizerDesc, &wireframeRasterizerState);
  if (FAILED(result))
  {
    Logger::error("Failed to create rasterizer state");
  }

  // bind the rasterizer state to the pipeline
  deviceContext->RSSetState(wireframeRasterizerState);
}

// set the rasterizer's fill mode with a default of fill all
void Graphics::Renderer::setRasterizerFillMode(D3D11_FILL_MODE fillMode)
{
  // set the fill mode
  rasterizerDesc.FillMode = fillMode;
  // recreate the rasterizer state - this will need to change once we finish setup of main rendering pipeline
  device->CreateRasterizerState(&rasterizerDesc, &wireframeRasterizerState);
  // re-bind the rasterizer state
  deviceContext->RSSetState(wireframeRasterizerState);
}

// clear the target view - this should be done every frame
void Graphics::Renderer::clearTargetView()
{
  deviceContext->ClearRenderTargetView(renderTargetView, backgroundColor);
}

// set the position and scale of our render viewport
void Graphics::Renderer::setRenderTargetProperties(float x, float y, float width, float height)
{
  // Set the viewport transform
  D3D11_VIEWPORT viewport = {};
  viewport.Width = width;
  viewport.Height = height;
  viewport.TopLeftX = x;
  viewport.TopLeftY = y;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  deviceContext->RSSetViewports(1, &viewport);
}

// set the background color 
void Graphics::Renderer::setBackgroundColor(float color[4])
{
  backgroundColor[0] = color[0];
  backgroundColor[1] = color[1];
  backgroundColor[2] = color[2];
  backgroundColor[3] = color[3];
}

void Graphics::Renderer::toggleDebugMode()
{
  debug = !debug;
}

bool Graphics::Renderer::isDebugMode()
{
  return debug;
}

// set the renderer's topology
void Graphics::Renderer::setTopology(D3D_PRIMITIVE_TOPOLOGY topology)
{
  deviceContext->IASetPrimitiveTopology(topology);
}

// add a new active point light
void Graphics::Renderer::addPointLight(PointLight* buff)
{
  if (lights > 7)
    return;

  pointLightsArray[lights] = buff;
  lights++;
}

// add a new active point light
void Graphics::Renderer::updatePointLight(PointLight* buff)
{
  for (int i = 0; i < lights; ++i)
  {
    if (pointLightsArray[i] == buff)
    {
      pointLightsArray[i] = buff;
    }
  }
}

// get the device
ID3D11Device* Graphics::Renderer::getDevice()
{
  return device;
}

ID3D11ShaderResourceView* Graphics::Renderer::GetGameTexture()
{
  return shaderResourceView;
}

void Graphics::Renderer::SetUVScale(float x, float y)
{
  drawDataBuffer->get().uvScale = { x,y };
  drawDataBuffer->updateAndBind();

}

// get the device context
ID3D11DeviceContext* Graphics::Renderer::getDeviceContext()
{
  return deviceContext;
}

void Graphics::Renderer::createImGuiSystem()
{
  glowGui = new Graphics::GlowGui(windowHandle,device,deviceContext,this);
}

ID3D11Texture2D* Graphics::Renderer::getBackBuffer()
{
  return backBuffer;
}
