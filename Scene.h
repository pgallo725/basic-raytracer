#pragma once

#include <fstream>

#include "Common.h"
#include "Camera.h"
#include "Sphere.h"


class Scene : public Hittable
{
	friend void from_json(const json& j, Scene& s);

public:

	Camera camera;
    std::vector<Sphere> spheres;

	// Only needed for deserialization
	Scene() {};

public:

	static Scene Load(const std::string& filename)
	{
		std::ifstream file(filename);
		json json_data;
		file >> json_data;
		file.close();

		return json_data.get<Scene>();
	}

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


// JSON deserialization function
void from_json(const json& j, Scene& s)
{
	j.at("camera").get_to<Camera>(s.camera);
	j.at("spheres").get_to<std::vector<Sphere>>(s.spheres);
}
