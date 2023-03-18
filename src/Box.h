#pragma once

#include "Common.h"
#include "Rectangle.h"


class Box : public Hittable
{
public:

	Point3 min;
	Point3 max;
	std::array<std::shared_ptr<Rectangle>, 6> rectangles;

public:

	Box() = default;
	Box(const Point3& p0, const Point3& p1, std::shared_ptr<Material> material)
		: min(p0), max(p1)
	{
		rectangles[0] = std::make_shared<Rectangle>(Point3{ p0.x(), p0.y(), p1.z() }, Point3{ p1.x(), p1.y(), p1.z() }, material);
		rectangles[1] = std::make_shared<Rectangle>(Point3{ p0.x(), p0.y(), p0.z() }, Point3{ p1.x(), p1.y(), p0.z() }, material);

		rectangles[2] = std::make_shared<Rectangle>(Point3{ p0.x(), p1.y(), p0.z() }, Point3{ p1.x(), p1.y(), p1.z() }, material);
		rectangles[3] = std::make_shared<Rectangle>(Point3{ p0.x(), p0.y(), p0.z() }, Point3{ p1.x(), p0.y(), p1.z() }, material);

		rectangles[4] = std::make_shared<Rectangle>(Point3{ p1.x(), p0.y(), p0.z() }, Point3{ p1.x(), p1.y(), p1.z() }, material);
		rectangles[5] = std::make_shared<Rectangle>(Point3{ p0.x(), p0.y(), p0.z() }, Point3{ p0.x(), p1.y(), p1.z() }, material);
	}


	virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit)
		const noexcept override final
	{
		HitRecord last_hit;
		bool hit_some_rectangle = false;
		double t_closest = t_max;

		for (const auto& rectangle : rectangles)
		{
			if (rectangle->Hit(ray, t_min, t_closest, last_hit))
			{
				t_closest = last_hit.t;
				hit_some_rectangle = true;
				hit = last_hit;
			}
		}

		return hit_some_rectangle;
	}


	virtual bool BoundingBox(const double /*t_start*/, const double /*t_end*/, AABB& box)
		const noexcept override final
	{
		box = AABB(min, max);
		return true;
	}
};