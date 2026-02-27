/*
/
// filename: LightBuffer.h
// author: Callen Betts
// brief: defines all of the different lightbuffers
/
*/

#pragma once
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

namespace Lighting
{

  // define world light
  struct GlobalLightBuffer
  {
    DirectX::XMFLOAT3 lightDirection;
    float padding;
    DirectX::XMFLOAT3 lightColor;
    float padding2;
    DirectX::XMFLOAT3 cameraPosition;
    float padding3;
  };

  // define a point light buffer
  struct PointLightBuffer
  {
    DirectX::XMFLOAT3 position;
    float size;
    DirectX::XMFLOAT4 color;
  };

  // define a shadow light buffer for global light
  struct ShadowLightBuffer
  {
    DirectX::XMMATRIX lightMatrix;
  };

  // define a point light class that contains the buffer
  class PointLight
  {

  public:

    PointLightBuffer pointLight;
    unsigned int id = 0;

  };

}

using namespace Lighting; // ease of use