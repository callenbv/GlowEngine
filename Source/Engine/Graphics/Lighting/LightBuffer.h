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
        DirectX::XMFLOAT4 lightDir_ws;
        DirectX::XMFLOAT4 lightColor;
        DirectX::XMFLOAT4 cameraPos_ws;
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