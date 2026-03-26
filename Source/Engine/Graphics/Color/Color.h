/*
/
// filename: Color.h
// author: Callen Betts
// brief: defines Color class
/
*/

#pragma once

class Color
{
public:

  Color(float red = 1, float green = 1, float blue = 1, float alpha = 1) : r(red), g(green), b(blue), a(alpha) {};

  float r, g, b, a;

  static const Color Red;
  static const Color Green;
  static const Color Blue;
  static const Color White;
  static const Color Clear;
  static const Color Outline;

};

struct ColorBuffer
{
    float r, g, b, a;
};
