#pragma once

#include "Common.h"
#include "AABB.h"


// Defines an abstract common interface for all objects that can be hit by a ray.
class Hittable
{
public:

    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit) const noexcept = 0;
    virtual bool BoundingBox(const double t_start, const double t_end, AABB& box) const noexcept = 0;
};
