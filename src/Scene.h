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

	// Only needed for deserialization
	Scene() {};

public:

    virtual bool Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const override;
};


// Checks ray-object intersection for all objects in the scene list and returns the closest one to the camera.
bool Scene::Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const
{
	HitRecord temp_hit;
	bool hit_something = false;
	double t_closest = t_max;

	for (const auto& sphere : spheres)
	{
		if (sphere.Hit(ray, t_min, t_closest, temp_hit))
		{
			t_closest = temp_hit.t;
			hit_something = true;
			hit = temp_hit;
		}
	}

	return hit_something;
}
