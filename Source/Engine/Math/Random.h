/*
/
// filename: Random.h
// author: Callen Betts
// brief: defines Random and Index structs
/
*/

#pragma once
#include <random>

namespace GlowMath
{

  // random range for integers
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, T>::type
    randomRange(T min, T max)
  {
    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<T> distribution(min, max - 1); // Note: max - 1 for inclusive range
    return distribution(generator);
  }

  // random ranges for any other type
  template <typename T>
  typename std::enable_if<std::is_floating_point<T>::value, T>::type
    randomRange(T min, T max)
  {
    static std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<T> distribution(min, max);
    return distribution(generator);
  }

}