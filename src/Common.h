#pragma once

#include <stdint.h>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>
#include <string>

// Common Headers

#include "Vector3.h"
#include "Random.h"
#include "Ray.h"
#include "HitRecord.h"

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

/* Compute texture coordinates of a point on a sphere.
    @param point  A given point on the unit sphere, centered at the origin.
    @param u      Returned value [0, 1] of angle around the Y axis from X = -1.
    @param v      Returned value [0, 1] of angle from Y = -1 to Y = +1.
*/
inline void GetSphereUV(const Point3& point, double& u, double& v)
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
    const double theta = std::acos(-point.y());
    const double phi = std::atan2(-point.z(), point.x()) + PI;

    u = phi / (2 * PI);     // phi is in [0, 2pi]
    v = theta / PI;         // theta is in [0, pi]
}
