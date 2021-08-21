#pragma once

#include "Vector3.h"

class Ray
{
public:

	Point3 origin;
	Vector3 direction;

public:

	Ray() {}
	Ray(const Point3& origin, const Vector3& direction)
		: origin(origin), direction(direction)
	{}

	Point3 At(const double t) const
	{
		// P(t) = A + t*b
		return origin + t * direction;
	}
};
