#pragma once

#include "Ray.h"

class Material;		// Forward declaration


// Contains the information about a ray-object intersection
struct HitRecord
{
	Point3 point;
	Vector3 normal;
	double t;
	bool front_face;
	std::shared_ptr<Material> material;

	inline void SetFaceNormal(const Ray& ray, const Vector3& outward_normal)
	{
		front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};


// Defines an abstract common interface for all objects that can be hit by a ray
class Hittable
{
public:

	virtual bool Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const = 0;
};
