/*
/
// filename: BoundingBox.cpp
// author: Callen Betts
// brief: implements BoundingBox.h
/
*/
#include "stdafx.h"
#include "BoundingBox.h"
#include <algorithm>

// set the bounding box's size
Components::BoundingBox::BoundingBox()
{
  name = "Bounding Box";
  type = ComponentType::BoundingBox;
  simulation = false;

  AddVariable(CreateVariable("Min Box Size", &min));
  AddVariable(CreateVariable("Max Box Size", &max));
}

void Components::BoundingBox::update()
{
  if (!parent->sprite)
    return;

  // update our bounding box to be the mesh
  Components::Transform transform = *parent->transform;

  // Find the model in the map
  const std::vector<Vertex> vertices = parent->sprite->getModel()->allVertices;

  if (vertices.empty())
    return;

  min = Vector3D(vertices[0].x, vertices[0].y, vertices[0].z);
  max = Vector3D(vertices[0].x, vertices[0].y, vertices[0].z);

  // Loop through all vertices to find the bounding box
  for (const auto& vertex : vertices) {
    if (vertex.x < min.x) min.x = vertex.x;
    if (vertex.y < min.y) min.y = vertex.y;
    if (vertex.z < min.z) min.z = vertex.z;
    if (vertex.x > max.x) max.x = vertex.x;
    if (vertex.y > max.y) max.y = vertex.y;
    if (vertex.z > max.z) max.z = vertex.z;
  }

  min = min + transform.getPosition() - transform.getScale();
  max = max + transform.getPosition() + transform.getScale();
}

Components::BoundingBox* Components::BoundingBox::clone()
{
  return new BoundingBox(*this);
}
