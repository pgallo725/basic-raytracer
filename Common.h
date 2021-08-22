#pragma once

#include <cmath>
#include <limits>
#include <memory>
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

inline Vector3 RandomVector(double min, double max)
{
    return Vector3(RandomDouble(min, max),
        RandomDouble(min, max),
        RandomDouble(min, max));
}

inline Vector3 RandomInUnitSphere()
{
    while (true) 
    {
        // Pick a random point in the unit cube, where x, y, and z in [-1, +1].
        Vector3 p = RandomVector(-1, 1);
        // Reject this point and try again if the point is outside the sphere.
        if (p.SqrLength() >= 1) continue;
        return p;
    }
}

inline Vector3 RandomInHemisphere(const Vector3& normal)
{
    Vector3 in_unit_sphere = RandomInUnitSphere();
    return Vector3::Dot(in_unit_sphere, normal) > 0.0 ?     // In the same hemisphere as the normal
        in_unit_sphere : -in_unit_sphere;
}

inline Vector3 RandomUnitVector()
{
    /*double rand_theta = RandomDouble(0.0, PI);
    double rand_psi = RandomDouble(0.0, 2 * PI);

    return Vector3(std::cos(rand_psi) * std::sin(rand_theta),
        std::sin(rand_psi) * std::sin(rand_theta),
        std::cos(rand_theta));*/

    return Vector3::Normalized(RandomInUnitSphere());
}

inline double Clamp(double x, double min, double max) 
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
