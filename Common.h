#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <vector>


// Common Headers

#include "Ray.h"
#include "Vector3.h"
#include "Random.h"


// Constants

const double Infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;


// Utility Functions

inline double Deg2Rad(double degrees) 
{
    return degrees * PI / 180.0;
}

inline double Rad2Deg(double radians) 
{
    return radians * 180.0 / PI;
}

inline double Clamp(double x, double min, double max) 
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
