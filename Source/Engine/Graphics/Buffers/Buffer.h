/*
/
// filename: ConstantBuffer.h
// author: Callen Betts
// brief: defines ConstantBuffer class
/
*/

#pragma once

namespace Graphics
{
  class Buffer
  {

  public:

    Buffer();
    ~Buffer();

    // lets us bind and update from the base class
    virtual void bind() {};
    virtual void update() {};

    void updateAndBind();
    void addToBufferList();

    bool isGlobal();

  protected:

    ID3D11Buffer* buffer;
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    ShaderType shaderType;
    UINT shaderSlot;
    bool global;
   
  };
}
