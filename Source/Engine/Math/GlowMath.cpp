/*
/
// filename: GlowMath.cpp
// author: Callen Betts
// brief: implements GlowMath.h
/
*/

#include "stdafx.h"
#include "GlowMath.h"
#include "Lerp.h"
#include "Engine/Graphics/Window/Window.h"
#include "Engine/Graphics/Camera/Camera.h"
#include "Engine/GlowEngine.h"

// method to turn screen coordinates into world coordinates; used for selecting objects or interacting with mouse
Vector3D GlowMath::Vector3D::ScreenToWorldCoords(Vector3D coords)
{
  int viewWidth = Graphics::Window::GetWidth();
  int viewHeight = Graphics::Window::GetHeight();

  Visual::Camera* camera = EngineInstance::getEngine()->getCamera();

  DirectX::XMMATRIX viewMatrix = camera->getViewMatrix();
  DirectX::XMMATRIX perspectiveMatrix = camera->getPerspecitveMatrix();

  // Convert screen coordinates to normalized device coordinates (NDC)
  float x = (2.0f * coords.x) / viewWidth - 1.0f;
  float y = 1.0f - (2.0f * coords.y) / viewHeight;
  float z = 2.0f * coords.z - 1.0f;

  // Convert NDC to clip space coordinates
  DirectX::XMVECTOR clipCoords = DirectX::XMVectorSet(x, y, z, 1.0f);

  // Inverse projection transformation
  DirectX::XMVECTOR eyeCoords = DirectX::XMVector4Transform(clipCoords, DirectX::XMMatrixInverse(nullptr, perspectiveMatrix));
  eyeCoords = DirectX::XMVectorSet(DirectX::XMVectorGetX(eyeCoords), DirectX::XMVectorGetY(eyeCoords), -1.0f, 1.0f);

  // Inverse view transformation
  DirectX::XMVECTOR worldCoords = DirectX::XMVector4Transform(eyeCoords, DirectX::XMMatrixInverse(nullptr, viewMatrix));
  worldCoords = DirectX::XMVectorScale(worldCoords, 1.0f / DirectX::XMVectorGetW(worldCoords));

  return Vector3D(DirectX::XMVectorGetX(worldCoords), DirectX::XMVectorGetY(worldCoords), DirectX::XMVectorGetZ(worldCoords));
}

Vector3D GlowMath::Vector3D::XMVectorToVector3D(DirectX::XMVECTOR vector)
{
  return Vector3D(DirectX::XMVectorGetX(vector), DirectX::XMVectorGetY(vector), DirectX::XMVectorGetZ(vector));
}

Vector3D GlowMath::Vector3D::XMFloatToVector3D(DirectX::XMFLOAT3 XMFloat)
{
  return Vector3D(XMFloat.x,XMFloat.y,XMFloat.z);
}

// given a ray, return whether or not it intersects with our bounding box
bool GlowMath::Vector3D::RayIntersectsBoundingBox(const Vector3D& rayOrigin, const Vector3D& rayDirection, const Components::BoundingBox* box, float& t)
{
  Vector3D min = box->min;
  Vector3D max = box->max;

  float tMin = (min.x - rayOrigin.x) / rayDirection.x;
  float tMax = (max.x - rayOrigin.x) / rayDirection.x;

  if (rayDirection.x == 0) {
    tMin = (min.x > rayOrigin.x || max.x < rayOrigin.x) ? -FLT_MAX : FLT_MAX;
    tMax = tMin;
  }

  if (tMin > tMax) std::swap(tMin, tMax);

  float tyMin = (min.y - rayOrigin.y) / rayDirection.y;
  float tyMax = (max.y - rayOrigin.y) / rayDirection.y;

  if (rayDirection.y == 0) {
    tyMin = (min.y > rayOrigin.y || max.y < rayOrigin.y) ? -FLT_MAX : FLT_MAX;
    tyMax = tyMin;
  }

  if (tyMin > tyMax) std::swap(tyMin, tyMax);

  if ((tMin > tyMax) || (tyMin > tMax))
    return false;

  if (tyMin > tMin)
    tMin = tyMin;

  if (tyMax < tMax)
    tMax = tyMax;

  float tzMin = (min.z - rayOrigin.z) / rayDirection.z;
  float tzMax = (max.z - rayOrigin.z) / rayDirection.z;

  if (rayDirection.z == 0) {
    tzMin = (min.z > rayOrigin.z || max.z < rayOrigin.z) ? -FLT_MAX : FLT_MAX;
    tzMax = tzMin;
  }

  if (tzMin > tzMax) std::swap(tzMin, tzMax);

  if ((tMin > tzMax) || (tzMin > tMax))
    return false;

  if (tzMin > tMin)
    tMin = tzMin;

  if (tzMax < tMax)
    tMax = tzMax;

  t = tMin;

  return true;
}

Vector3D GlowMath::Vector3D::ViewToWorldSpace(const Vector3D& viewCoords, const DirectX::XMMATRIX& inverseView)
{
  DirectX::XMVECTOR view = DirectX::XMVectorSet(viewCoords.x, viewCoords.y, viewCoords.z, 0.0f);
  DirectX::XMVECTOR worldSpace = DirectX::XMVector4Transform(view, inverseView);
  DirectX::XMFLOAT3 worldSpaceCoords;
  DirectX::XMStoreFloat3(&worldSpaceCoords, worldSpace);
  return Vector3D(worldSpaceCoords.x, worldSpaceCoords.y, worldSpaceCoords.z);
}

Vector3D GlowMath::Vector3D::NDCToViewSpace(const Vector3D& ndcCoords, const DirectX::XMMATRIX inverseProjection)
{
  DirectX::XMVECTOR ndc = DirectX::XMVectorSet(ndcCoords.x, ndcCoords.y, ndcCoords.z, 1.0f);
  DirectX::XMVECTOR viewSpace = DirectX::XMVector4Transform(ndc, inverseProjection);
  DirectX::XMFLOAT3 viewSpaceCoords;
  DirectX::XMStoreFloat3(&viewSpaceCoords, viewSpace);
  return Vector3D(viewSpaceCoords.x, viewSpaceCoords.y, viewSpaceCoords.z);
}

Vector3D GlowMath::Vector3D::ScreenToNDC(const ImVec2& screenCoords, const ImVec2& screenSize)
{
  float ndcX = (2.0f * screenCoords.x) / screenSize.x - 1.0f;
  float ndcY = 1.0f - (2.0f * screenCoords.y) / screenSize.y;
  return Vector3D(ndcX, ndcY, 0.0f);
}

GlowMath::Vector3D::Vector3D(float x, float y, float z)
  :
  x(x),
  y(y),
  z(z)
{
}

void GlowMath::Vector3D::operator+=(const Vector3D& other)
{
  x += other.x;
  y += other.y;
  z += other.z;
}

void GlowMath::Vector3D::operator-=(const Vector3D& other)
{
  x -= other.x;
  y -= other.y;
  z -= other.z;
}

Vector3D GlowMath::Vector3D::operator+(const Vector3D& other)
{
  return Vector3D(x + other.x, y + other.y, z + other.z);
}

Vector3D GlowMath::Vector3D::operator-(const Vector3D& other)
{
  return Vector3D(x - other.x, y - other.y, z - other.z);
}

void GlowMath::Vector3D::lerpTowards(const Vector3D& targetVector, float t)
{
  x = Lerper::Ease(x, targetVector.x, t);
  y = Lerper::Ease(y, targetVector.y, t);
  z = Lerper::Ease(z, targetVector.z, t);
}

void GlowMath::Vector3D::normalize()
{
  float length = std::sqrt(x * x + y * y + z * z);
  if (length > 0.0f) 
  {  
    x /= length;
    y /= length;
    z /= length;
  }
}

float GlowMath::Vector3D::dot(const Vector3D other)
{
  return x * other.x + y * other.y + z * other.z;
}

Vector3D GlowMath::Vector3D::operator*(const Vector3D& other)
{
  return Vector3D(x * other.x, y * other.y, z * other.z);
}

Vector3D GlowMath::Vector3D::operator*(const float& other)
{
  return Vector3D(x * other, y * other, z * other);
}

bool GlowMath::Vector3D::operator==(const float& other)
{
  return (x == other && y == other && z == other);
}

Vector3D GlowMath::Vector3D::operator/(const Vector3D& other)
{
  return Vector3D(x / other.x, y / other.y, z / other.z);
}

