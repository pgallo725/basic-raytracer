#pragma once

#include "Hittable.h"

#include <memory>
#include <vector>
#include <iterator>


class HittableList : public Hittable
{
public:

	std::vector<std::shared_ptr<Hittable>> objects;

public:

	HittableList() {}
	HittableList(std::shared_ptr<Hittable> object) { Add(object); }
	HittableList(std::vector<std::shared_ptr<Hittable>>::const_iterator start,
		std::vector<std::shared_ptr<Hittable>>::const_iterator end) { Add(start, end); }

	void Clear() { objects.clear(); }
	void Add(std::shared_ptr<Hittable> object) { objects.push_back(object); }
	void Add(std::vector<std::shared_ptr<Hittable>>::const_iterator start,
		std::vector<std::shared_ptr<Hittable>>::const_iterator end) 
	{ 
		auto it = start;
		while (it != end)
		{
			objects.push_back(*it);
			(it)++;
		}
	}

	virtual bool Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const override;
};


// Checks ray-object intersection for all objects in the hittable list and returns the closest one to the camera.
bool HittableList::Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const
{
	HitRecord temp_hit;
	bool hit_something = false;
	double t_closest = t_max;

	for (const auto& object : objects)
	{
		if (object->Hit(ray, t_min, t_closest, temp_hit))
		{
			t_closest = temp_hit.t;
			hit_something = true;
			hit = temp_hit;
		}
	}

	return hit_something;
}
