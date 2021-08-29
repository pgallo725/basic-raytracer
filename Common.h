#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <random>


// Common Headers

#include "Ray.h"
#include "Vector3.h"


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

inline double RandomDouble(double min, double max) 
{
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double Clamp(double x, double min, double max) 
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
