/*
/
// filename: BoundingBox.h
// author: Callen Betts
// brief: defines BoundingBox class
/
*/

#pragma once
#include "Engine/Entity/Components/Component.h"

namespace Components
{
  class BoundingBox : public Component
  {

  public:

    BoundingBox();

    void update();
    BoundingBox* clone();

    Vector3D max;
    Vector3D min;

  private:


  };
}