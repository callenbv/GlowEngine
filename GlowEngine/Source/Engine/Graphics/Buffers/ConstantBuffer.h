/*
/
// filename: ConstantBuffer.h
// author: Callen Betts
// brief: defines ConstantBuffer class
/
*/

#pragma once
#include "Buffer.h"

struct cbPerObject
{
  DirectX::XMMATRIX world;
  DirectX::XMMATRIX view;
  DirectX::XMMATRIX projection;
};

struct DrawDataBuffer
{
  DirectX::XMFLOAT2 uvScale;
  float padding;
  float padding2;
};

namespace Graphics
{
  template <typename T>
  class ConstantBuffer : public Buffer
  {

  public:

    // create the buffer and bind it in one go; it can be bound differently later if needed
    ConstantBuffer(ID3D11Device* device_, ID3D11DeviceContext* context_, UINT slot, bool isGlobal, ShaderType type = ShaderType::Pixel) : Buffer()
    {
      buffer = nullptr;
      device = device_;
      context = context_;
      shaderSlot = slot;
      global = isGlobal;
      shaderType = type;

      D3D11_BUFFER_DESC desc = {};
      desc.Usage = D3D11_USAGE_DYNAMIC;
      desc.ByteWidth = sizeof(T); // if we have a buffer of several objects, then we need a multiplier
      desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

      HRESULT hr = device->CreateBuffer(&desc, nullptr, &buffer);

      if (FAILED(hr))
      {
        throw std::runtime_error("Failed to create constant buffer");
      }
    }

    // delete the buffer and release it
    ~ConstantBuffer()
    {
      if (buffer)
        buffer->Release();
    }

    // map the buffer to the context
    void update()
    {
      D3D11_MAPPED_SUBRESOURCE mappedResource;
      HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

      if (FAILED(hr))
      {
        throw std::runtime_error("Failed to map constant buffer");
      }

      memcpy(mappedResource.pData, &data, sizeof(T));
      context->Unmap(buffer, 0);
    }

    // bind the buffer to a slot
    void bind()
    {
      switch (shaderType)
      {
      case ShaderType::Pixel:
        context->PSSetConstantBuffers(shaderSlot, 1, &buffer);
        break;

      case ShaderType::Vertex:
        context->VSSetConstantBuffers(shaderSlot, 1, &buffer);
        break;
      }
    }

    // rebind the buffer if we need to do it later
    void reBind(UINT slot, ShaderType type)
    {
      shaderSlot = slot;
      shaderType = type;

      bind();
    }

    // get the struct data 
    T& get()
    {
      return data;
    }

    // set the data to an instance of a struct
    void set(const T& structData)
    {
      data = structData;
    }

  private:

    // struct data is auto created, this can be changed using set() later
    // retrieved using "get"
    T data = {};

  };
}
