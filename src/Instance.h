#pragma once

#include "Common.h"
#include "Hittable.h"


class Translate : public Hittable
{
public:

	Vector3 offset;
	std::shared_ptr<Hittable> object;

public:

	Translate(std::shared_ptr<Hittable> obj, const Vector3& offset)
		: offset(offset), object(obj)
	{}


	// Ray-Object intersection checking with translation.
	virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit)
		const noexcept override final
	{
		// Instead of actually translating the object, we translate the ray
		// in the opposite direction before intersecting it with the object
		// (which stays at its original position)
		Ray ray_translated(ray.origin - offset, ray.direction, ray.time);
		if (!object->Hit(ray_translated, t_min, t_max, hit))
			return false;

		// Translate back the hit point
		hit.point += offset;
		return true;
	}


	// Translated bounding box.
	virtual bool BoundingBox(const double t_start, const double t_end, AABB& box)
		const noexcept override final
	{
		if (!object->BoundingBox(t_start, t_end, box))
			return false;

		box.min += offset;
		box.max += offset;
		return true;
	}
};


class Rotate_Y : public Hittable
{
public:

	double sin_theta;
	double cos_theta;
	bool has_box;
	AABB bbox;
	std::shared_ptr<Hittable> object;

public:

	Rotate_Y(std::shared_ptr<Hittable> obj, const double angle)
		: object(obj)
	{
		const double radians = Deg2Rad(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		has_box = object->BoundingBox(0.0, 1.0, bbox);	// TODO: t_start and t_end should be parameterized

		Point3 min( Infinity,  Infinity,  Infinity);
		Point3 max(-Infinity, -Infinity, -Infinity);

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					const double x = i * bbox.max.x() + (1 - i) * bbox.min.x();
					const double y = j * bbox.max.y() + (1 - j) * bbox.min.y();
					const double z = k * bbox.max.z() + (1 - k) * bbox.min.z();

					const double newx =  cos_theta * x + sin_theta * z;
					const double newz = -sin_theta * x + cos_theta * z;

					min[0] = std::min(min[0], newx);
					min[1] = std::min(min[1], y);
					min[2] = std::min(min[2], newz);
					max[0] = std::max(max[0], newx);
					max[1] = std::max(max[1], y);
					max[2] = std::max(max[2], newz);
				}
			}
		}

		bbox = AABB(min, max);
	}


	// Ray-Object intersection checking with rotation around the Y axis.
	virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit)
		const noexcept override final
	{
		Point3  origin = ray.origin;
		Vector3 direction = ray.direction;

		// Rotate the ray origin and direction around the Y axis
		origin[0] = cos_theta * ray.origin[0] - sin_theta * ray.origin[2];
		origin[2] = sin_theta * ray.origin[0] + cos_theta * ray.origin[2];
		direction[0] = cos_theta * ray.direction[0] - sin_theta * ray.direction[2];
		direction[2] = sin_theta * ray.direction[0] + cos_theta * ray.direction[2];

		Ray ray_rotated(origin, direction, ray.time);

		if (!object->Hit(ray_rotated, t_min, t_max, hit))
			return false;

		Point3 point = hit.point;
		Vector3 normal = hit.normal;

		// Rotate back the hit point and surface normal
		point[0] =  cos_theta * hit.point[0] + sin_theta * hit.point[2];
		point[2] = -sin_theta * hit.point[0] + cos_theta * hit.point[2];
		normal[0] =  cos_theta * hit.normal[0] + sin_theta * hit.normal[2];
		normal[2] = -sin_theta * hit.normal[0] + cos_theta * hit.normal[2];

		hit.point = point;
		hit.is_front_face = Vector3::Dot(ray_rotated.direction, normal) < 0.0;
		hit.normal = hit.is_front_face ? normal : -normal;
		return true;
	}


	// Rotated bounding box.
	virtual bool BoundingBox(const double /*t_start*/, const double /*t_end*/, AABB& box)
		const noexcept override final
	{
		box = bbox;
		return has_box;
	}
};