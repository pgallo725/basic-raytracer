#pragma once

#include <cassert>

#include "Common.h"
#include "Hittable.h"


class Rectangle : public Hittable
{
public:

	enum class Type
	{
		Invalid,
		XY,
		XZ,
		YZ
	};

	Type type = Type::Invalid;
	double k  = 0.0;
	double a0 = 0.0, b0 = 0.0;
	double a1 = 0.0, b1 = 0.0;
	std::shared_ptr<Material> material;

public:

	Rectangle(const Point3& p0, const Point3& p1, std::shared_ptr<Material> material)
		: material(material)
	{
		if (p0.x() == p1.x())
		{
			type = Type::YZ;
			k  = p0.x();
			a0 = p0.y();
			b0 = p0.z();
			a1 = p1.y();
			b1 = p1.z();
		}
		else if (p0.y() == p1.y())
		{
			type = Type::XZ;
			k  = p0.y();
			a0 = p0.x();
			b0 = p0.z();
			a1 = p1.x();
			b1 = p1.z();
		}
		else if (p0.z() == p1.z())
		{
			type = Type::XY;
			k  = p0.z();
			a0 = p0.x();
			b0 = p0.y();
			a1 = p1.x();
			b1 = p1.y();
		}
		else
		{
			std::cerr << "Only axis-aligned rectangles are supported.\n";
			assert(false);
		}
	}


	// Ray-Rectangle (axis aligned) intersection checking.
	virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit)
		const noexcept override final
	{
		switch (type)
		{
			case Type::XY:
			{
				// We solve the ray equation P(t) = A + t*b to find out the
				// value of t where Pz == k, we can then use this value to calculate
				// the x and y coordinates at the intersection point.
				const double t = (k - ray.origin.z()) / ray.direction.z();
				if (t < t_min || t > t_max)
					return false;

				const double x = ray.origin.x() + t * ray.direction.x();
				const double y = ray.origin.y() + t * ray.direction.y();
				if (x < a0 || x > a1 || y < b0 || y > b1)
					return false;

				hit.t = t;
				hit.point = Point3(x, y, k);
				hit.u = (x - a0) / (a1 - a0);
				hit.v = (y - b0) / (b1 - b0);
				const Vector3 outward_normal = Vector3(0.0, 0.0, 1.0);
				hit.is_front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
				hit.normal = hit.is_front_face ? outward_normal : -outward_normal;
				hit.material = material.get();
				return true;
			}

			case Type::XZ:
			{
				// We solve the ray equation P(t) = A + t*b to find out the
				// value of t where Py == k, we can then use this value to calculate
				// the x and z coordinates at the intersection point.
				const double t = (k - ray.origin.y()) / ray.direction.y();
				if (t < t_min || t > t_max)
					return false;

				const double x = ray.origin.x() + t * ray.direction.x();
				const double z = ray.origin.z() + t * ray.direction.z();
				if (x < a0 || x > a1 || z < b0 || z > b1)
					return false;

				hit.t = t;
				hit.point = Point3(x, k, z);
				hit.u = (x - a0) / (a1 - a0);
				hit.v = (z - b0) / (b1 - b0);
				const Vector3 outward_normal = Vector3(0.0, 1.0, 0.0);
				hit.is_front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
				hit.normal = hit.is_front_face ? outward_normal : -outward_normal;
				hit.material = material.get();
				return true;
			}

			case Type::YZ:
			{
				// We solve the ray equation P(t) = A + t*b to find out the
				// value of t where Px == k, we can then use this value to calculate
				// the y and z coordinates at the intersection point.
				const double t = (k - ray.origin.x()) / ray.direction.x();
				if (t < t_min || t > t_max)
					return false;

				const double y = ray.origin.y() + t * ray.direction.y();
				const double z = ray.origin.z() + t * ray.direction.z();
				if (y < a0 || y > a1 || z < b0 || z > b1)
					return false;

				hit.t = t;
				hit.point = Point3(k, y, z);
				hit.u = (y - a0) / (a1 - a0);
				hit.v = (z - b0) / (b1 - b0);
				const Vector3 outward_normal = Vector3(1.0, 0.0, 0.0);
				hit.is_front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
				hit.normal = hit.is_front_face ? outward_normal : -outward_normal;
				hit.material = material.get();
				return true;
			}

			default: return false;
		}
	}


	// Rectangle bounding box.
	virtual bool BoundingBox(const double /*t_start*/, const double /*t_end*/, AABB& box)
		const noexcept override final
	{
		// The bounding box must have non-zero width in each dimension,
		//  so pad the constant dimension by a small amount.
		switch (type)
		{
			case Type::XY: box = AABB(Point3(a0, b0, k - 0.0001), Point3(a1, b1, k + 0.0001)); return true;
			case Type::XZ: box = AABB(Point3(a0, k - 0.0001, b0), Point3(a1, k + 0.0001, b1)); return true;
			case Type::YZ: box = AABB(Point3(k - 0.0001, a0, b0), Point3(k + 0.0001, a1, b1)); return true;
			default: return false;
		}
	}
};
