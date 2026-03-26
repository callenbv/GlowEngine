/*
/
// filename: Bone.cpp
// author: Callen Betts
// brief: implements Bone.h
/
*/

#include "stdafx.h"
#include "Bone.h"

Components::Bone::Bone()
  :
  parent(nullptr)
{
  // set the transform parts of the bone to identity matrix
  localTransform = DirectX::XMMatrixIdentity();
  globalTransform = DirectX::XMMatrixIdentity();
}
