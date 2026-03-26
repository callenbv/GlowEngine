/*
/
// filename: ConstantBuffer.cpp
// author: Callen Betts
// brief: implements ConstantBuffer.h
/
*/

#include "stdafx.h"
#include "ConstantBuffer.h"
#include "Engine/GlowEngine.h"
#include "Engine/Graphics/Renderer.h"

Graphics::Buffer::Buffer()
{
}

Graphics::Buffer::~Buffer()
{
  buffer->Release();
}

void Graphics::Buffer::updateAndBind()
{
  update();
  bind();
}

void Graphics::Buffer::addToBufferList()
{
  // automatically add the buffer to the list on creation
  Graphics::Renderer* renderer = EngineInstance::getEngine()->getRenderer();

  renderer->addBuffer(this);
}

bool Graphics::Buffer::isGlobal()
{
  return global;
}
