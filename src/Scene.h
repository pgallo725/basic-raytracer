#pragma once

#include "Common.h"
#include "Hittable.h"
#include "Camera.h"
#include "Sphere.h"


class Scene : public Hittable
{
public:

	Camera camera;
    std::vector<Sphere> spheres;

public:

	// Checks ray-object intersection for all objects in the scene list and returns the closest one to the camera.
	virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit) 
		const noexcept override final
	{
		HitRecord last_hit;
		bool hit_something = false;
		double t_closest = t_max;

		for (const auto& sphere : spheres)
		{
			if (sphere.Hit(ray, t_min, t_closest, last_hit))
			{
				t_closest = last_hit.t;
				hit_something = true;
				hit = last_hit;
			}
		}

		return hit_something;
	}
};
