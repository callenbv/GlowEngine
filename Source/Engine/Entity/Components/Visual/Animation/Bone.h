/*
/
// filename: Bone.h
// author: Callen Betts
// brief: defines the Bone class that contains our animation's transform data
/
*/

#pragma once

namespace Components
{
  
  class Bone : public Component
  {

  public:
    
    Bone();

    // set and get the parent bone
    void setParent(Bone* newParent) { parent = newParent; }
    Bone* getParent() { return parent; }

    // set the local transform matrix
    void setLocalTransform(Matrix matrix) { localTransform = matrix; }
    // set the global transform matrix
    void setGlobalTransform(Matrix matrix) { globalTransform = matrix; }

  private:

    Matrix localTransform;
    Matrix globalTransform;

    Bone* parent;

  };

}