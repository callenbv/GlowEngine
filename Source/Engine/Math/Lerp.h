/*
/
// filename: Lerp.h
// author: Callen Betts
// brief: defines Lerp functions
/
*/

#pragma once

namespace GlowMath
{
  class Lerper
  {
  public:
    // Linear interpolation function
    static float Ease(float start, float end, float t = .15f)
    {
      return start + t * (end - start);
    }

    // Easing in and out interpolation function
    static float EaseInOut(float start, float end, float t = .15f)
    {
      t = t * t * (3 - 2 * t);
      return Ease(start, end, t);
    }
  };
}