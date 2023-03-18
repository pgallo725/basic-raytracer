#pragma once

#include "Common.h"
#include "Hittable.h"
#include "Camera.h"
#include "Sphere.h"
#include "MovingSphere.h"
#include "BVH.h"


class Scene
{
public:

	Color background;
	Camera camera;
    std::vector<std::shared_ptr<Hittable>> objects;
	std::shared_ptr<NodeBVH> bvh;

public:

	// Checks ray-object intersection for all objects in the scene list and returns the closest one to the camera.
	bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit) const noexcept
	{
		if (bvh)
		{
			return bvh->Hit(ray, t_min, t_max, hit);
		}
		else
		{
			HitRecord last_hit;
			bool hit_something = false;
			double t_closest = t_max;

			for (const auto& object : objects)
			{
				if (object->Hit(ray, t_min, t_closest, last_hit))
				{
					t_closest = last_hit.t;
					hit_something = true;
					hit = last_hit;
				}
			}

			return hit_something;
		}
	}


	bool BoundingBox(const double t_start, const double t_end, AABB& box) const noexcept
	{
		if (objects.empty())
			return false;

		box = AABB(
			{
			  std::numeric_limits<double>::max(),
			  std::numeric_limits<double>::max(),
			  std::numeric_limits<double>::max() 
			},
			{
			  std::numeric_limits<double>::lowest(),
			  std::numeric_limits<double>::lowest(),
			  std::numeric_limits<double>::lowest()
			}
		);
		AABB temp_box;	// to store intermediate results

		for (const auto& object : objects)
		{
			object->BoundingBox(t_start, t_end, temp_box);
			box = AABB::Combine(box, temp_box);
		}
	}


	// Builds Bounding Volume Hierarchy (BVH) structure for accelerating ray intersection tests.
	void BuildBVH(const double t_start, const double t_end) noexcept
	{
		bvh = std::make_shared<NodeBVH>(objects, 0, objects.size(), t_start, t_end);
	}
};
