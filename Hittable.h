#pragma once

#include "Ray.h"

class Material;		// Forward declaration


// Contains the information about a ray-object intersection
struct HitRecord
{
	double		t;
	Point3		point;
	Vector3		normal;
	bool		is_front_face;
	Material*	material;
};


// Defines an abstract common interface for all objects that can be hit by a ray
class Hittable
{
public:

	virtual bool Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const = 0;
};
