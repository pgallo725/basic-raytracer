#pragma once

#include "Common.h"


// Defines an abstract common interface for all objects that can be hit by a ray
class Hittable
{
public:

    virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit) const = 0;
};
