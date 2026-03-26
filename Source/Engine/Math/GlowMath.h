/*
/
// filename: GlowMath.h
// author: Callen Betts
// brief: defines GlowMath class
/
*/

#pragma once

namespace Components
{
  class BoundingBox;
}

struct ImVec2;

namespace GlowMath
{

  // vector
  class Vector3D
  {

  public:

    // convert a vector to world coordinates
    static Vector3D ScreenToWorldCoords(Vector3D coords);
    static Vector3D XMVectorToVector3D(DirectX::XMVECTOR vector);
    static Vector3D XMFloatToVector3D(DirectX::XMFLOAT3 XMFloat);
    static bool RayIntersectsBoundingBox(const Vector3D& rayOrigin, const Vector3D& rayDirection, const Components::BoundingBox* box, float& t);
    static Vector3D ViewToWorldSpace(const Vector3D& viewCoords, const DirectX::XMMATRIX& inverseView);
    static Vector3D NDCToViewSpace(const Vector3D& ndccoords, const DirectX::XMMATRIX inverseProjection);
    static Vector3D ScreenToNDC(const ImVec2& screenCoords, const ImVec2& screenSize);

    // default of zeroed vector
    Vector3D(float x = 0, float y = 0, float z = 0);

    // plus equals operator
    void operator+=(const Vector3D& other);
    void operator-=(const Vector3D& other);

    // plus operator
    Vector3D operator+(const Vector3D& other);
    Vector3D operator-(const Vector3D& other);

    // interpolation
    void lerpTowards(const Vector3D& targetVector, float t = .15f);

    // normalize
    void normalize();
    float dot(const Vector3D other);

    // multiply operator
    Vector3D operator*(const Vector3D& other);
    Vector3D operator*(const float& other);

    bool operator==(const float& other);

    // division
    Vector3D operator/(const Vector3D& other);

  public:

    // client can change vector properties
    float x, y, z;

  };

  // typdefs for better readability
  typedef DirectX::XMVECTOR XMVector;
  typedef DirectX::XMMATRIX Matrix;
  typedef GlowMath::Vector3D Vector3D;

}