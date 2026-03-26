/*
/
// filename: Vertex.h
// author: Callen Betts
// brief: defines Vertex and Index structs
/
*/

#pragma once

namespace GlowMath
{

  // define vertex struct
  struct Vertex
  {
    float x, y, z; // vertices
    float r, g, b, a; // color
    float nx, ny, nz; // normals
    float tx, ty; // texture coordinates
  };

  // define index struct
  struct Index 
  {
    int vertexIndex; // vertex index
    int textureIndex; // texture coordinate index
    int normalIndex; // normal vertex index
  };

}