#pragma once

#include "Vector3.h"

class Material;		// Forward declaration


// Contains the information about a ray-object intersection.
struct HitRecord
{
    double           t = 0.0;
    double           u = 0.0;
    double           v = 0.0;
    Point3           point;
    Vector3          normal;
    bool             is_front_face = false;
    const Material*  material = nullptr;
};
