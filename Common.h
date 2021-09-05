#pragma once

#include <stdint.h>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>


// Common Headers

#include "Ray.h"
#include "Vector3.h"
#include "Random.h"


// Constants

constexpr double Infinity = std::numeric_limits<double>::infinity();
constexpr double PI = 3.1415926535897932385;


// Utility Functions

constexpr double Deg2Rad(double degrees)
{
    return degrees * PI / 180.0;
}

constexpr double Rad2Deg(double radians)
{
    return radians * 180.0 / PI;
}

constexpr double Clamp(double x, double min, double max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
