#pragma once

#include "Common.h"


class AABB
{
public:

	Point3 min;
	Point3 max;

public:

	AABB() = default;
	AABB(const Point3& min, const Point3& max) 
		: min(min), max(max)
	{}

    inline bool Hit(const Ray& ray, double t_min, double t_max) const noexcept
    {
        for (int a = 0; a < 3; a++)
        {
            double invD = 1.0 / ray.direction[a];
            double t0 = (min[a] - ray.origin[a]) * invD;
            double t1 = (max[a] - ray.origin[a]) * invD;
            if (invD < 0.0)
                std::swap(t0, t1);
            t_min = std::max(t0, t_min);
            t_max = std::min(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return true;
    }


    static AABB Combine(const AABB& a, const AABB& b)
    {
        const Point3 min = { std::min(a.min.x(), b.min.x()),
                             std::min(a.min.y(), b.min.y()),
                             std::min(a.min.z(), b.min.z()) };
        const Point3 max = { std::max(a.max.x(), b.max.x()),
                             std::max(a.max.y(), b.max.y()),
                             std::max(a.max.z(), b.max.z()) };
        return AABB(min, max);
    }
};