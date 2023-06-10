#pragma once

#include "Vector3.h"


// Contains the information about a ray-object intersection.
struct HitRecord
{
    using ObjectID = uint32_t;
    using MaterialID = uint32_t;

    ObjectID         object_id = ~0u;
    MaterialID       material_id = ~0u;
    float            t = 0.0;
    float            u = 0.0;
    float            v = 0.0;
    Point3           point;
    Vector3          normal;
    bool             is_front_face = false;
};
