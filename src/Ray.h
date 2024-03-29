#pragma once

#include "Vector3.h"

struct Ray
{
	Point3 origin;
	Vector3 direction;
	double time = 0.0;

	Ray() = default;
	Ray(const Point3& origin, const Vector3& direction, double time)
		: origin(origin), direction(direction), time(time)
	{}

	Point3 At(const double t) const noexcept
	{
		// P(t) = A + t * b
		return origin + t * direction;
	}
};
