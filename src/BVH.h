#pragma once

#include "Common.h"
#include "Hittable.h"
#include "AABB.h"


class NodeBVH : public Hittable
{
public:

	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;
	AABB box;

public:

	NodeBVH() = default;

	// Build the BVH sub-tree structure for the given objects
	NodeBVH(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end, double time_start, double time_end)
	{
		// Create a modifiable array of the source scene objects
		auto objects = std::vector<std::shared_ptr<Hittable>>(src_objects.begin() + start, src_objects.begin() + end);

		if (objects.size() == 1)
		{
			left = right = objects[0];
		}
		else if (objects.size() == 2)
		{
			left = objects[0];
			right = objects[1];
		}
		else
		{
			int axis = Random::GetInteger(0, 2);
			const auto comparator = [axis](const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) -> bool
			{
				AABB box_a;
				AABB box_b;

				if (!a->BoundingBox(0, 0, box_a) || !b->BoundingBox(0, 0, box_b))
					std::cerr << "No bounding box in NodeBVH constructor.\n";

				return box_a.min[axis] < box_b.min[axis];
			};

			std::sort(objects.begin(), objects.end(), comparator);

			const size_t mid = objects.size() / 2;
			left = std::make_shared<NodeBVH>(objects, 0, mid, time_start, time_end);
			right = std::make_shared<NodeBVH>(objects, mid, objects.size(), time_start, time_end);
		}

		AABB box_left, box_right;

		if (!left->BoundingBox(time_start, time_end, box_left) || !right->BoundingBox(time_start, time_end, box_right))
			std::cerr << "No bounding box in NodeBVH constructor.\n";

		box = AABB::Combine(box_left, box_right);
	}


	virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit)
		const noexcept override final
	{
		if (!box.Hit(ray, t_min, t_max))
			return false;

		const bool hit_left = left->Hit(ray, t_min, t_max, hit);
		const bool hit_right = right->Hit(ray, t_min, hit_left ? hit.t : t_max, hit);

		return hit_left || hit_right;
	}


	virtual bool BoundingBox(const double /*t_start*/, const double /*t_end*/, AABB& output_box)
		const noexcept override final
	{
		output_box = this->box;
		return true;
	}
};