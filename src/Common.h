#pragma once

#include <stdint.h>
#include <malloc.h>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <variant>
#include <map>

// Common Headers

#include "Buffer.h"
#include "Vector3.h"
#include "Random.h"
#include "Ray.h"
#include "HitRecord.h"

// Constants

constexpr float Infinity = std::numeric_limits<float>::infinity();
constexpr float PI = 3.1415926535897932385;

// Type Aliases

using NodeID = uint32_t;
using ObjectID = uint32_t;
using MaterialID = uint32_t;

// Utility Functions

constexpr float Deg2Rad(float degrees)
{
    return degrees * PI / 180.0;
}

constexpr float Rad2Deg(float radians)
{
    return radians * 180.0 / PI;
}

constexpr float Clamp(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

/* Compute texture coordinates of a point on a sphere.
    @param point  A given point on the unit sphere, centered at the origin.
    @param u      Returned value [0, 1] of angle around the Y axis from X = -1.
    @param v      Returned value [0, 1] of angle from Y = -1 to Y = +1.
*/
inline void GetSphereUV(const Point3& point, float& u, float& v)
{
    // Texture coordinates U and V are obtained by mapping the (theta, phi) angles
    // of spherical coordinates to the range [0, 1].
    // To compute theta and phi for a given point on the unit sphere centered at the origin,
    // we can invert the equations for the corresponding Cartesian coordinates:
    //     y = -cos(theta)
    //     x = -cos(phi) * sin(theta)
    //     z =  sin(phi) * cos(theta)
    // Now, atan2() returns values in [-pi, pi] but they go from 0 to pi and then flip
    // to -pi and proceed back to 0. To get a contiguous interval value we can use the following
    // formulation instead: atan2(a, b) = atan2(-a, -b) + pi.
    const float theta = std::acos(-point.y());
    const float phi = std::atan2(-point.z(), point.x()) + PI;

    u = phi / (2 * PI);     // phi is in [0, 2pi]
    v = theta / PI;         // theta is in [0, pi]
}
