#pragma once

#include <cmath>
#include <limits>
#include <memory>

// Constants

const double Infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility Functions

inline double Deg2Rad(double degrees) {
    return degrees * PI / 180.0;
}

inline double Rad2Deg(double radians) {
    return radians * 180.0 / PI;
}

// Common Headers

#include "Ray.h"
#include "Vector3.h"

