#pragma once

#include "Common.h"
#include "Hittable.h"
#include "Camera.h"
#include "Sphere.h"
#include "MovingSphere.h"


class Scene
{
public:

	Camera camera;
    std::vector<std::shared_ptr<Hittable>> objects;

public:

	// Checks ray-object intersection for all objects in the scene list and returns the closest one to the camera.
	bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit) const noexcept
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
};
