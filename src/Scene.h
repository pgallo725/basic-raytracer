#pragma once

#include "Common.h"
#include "Camera.h"
#include "Hittable.h"
#include "Material.h"
#include "AABB.h"


class Scene
{
public:

	struct NodeBVH
	{
		AABB bbox;							// 24 bytes
		union index {						//  4 bytes
			NodeID left_child = ~0u;
			ObjectID first_object;
		} idx;
		uint32_t count = 0;					//  4 bytes
											//----------
											// 32 bytes

		bool IsLeaf() const noexcept { return count > 0; }
	};

public:

	Color background;
	Camera camera;
	Buffer<Material, 64> materials;
    Buffer<Hittable, 64> objects;
	Buffer<NodeBVH,  64> bvh_nodes;

private:

	static constexpr NodeID BVH_ROOT_ID = 1;	// skip first element in BVH node array, to have left-right children in same cache line
	NodeID bvh_node_id = BVH_ROOT_ID;


public:

	// Checks ray-object intersection for all objects in the scene list and returns the closest one to the camera.
	bool Intersect(const Ray& ray, const float t_min, const float t_max, HitRecord& hit) const noexcept
	{
		return (bvh_nodes.GetCount() > 0)
			? IntersectBVH(BVH_ROOT_ID, ray, t_min, t_max, hit)
			: IntersectObjects(0, objects.GetCount(), ray, t_min, t_max, hit);
	}


	// Scene bounding box, calculated by combining the bboxes of each object.
	bool BoundingBox(const float t_start, const float t_end, AABB& box) const noexcept
	{
		if (objects.GetCount() == 0)
			return false;

		box = AABB(
			{
			  std::numeric_limits<float>::max(),
			  std::numeric_limits<float>::max(),
			  std::numeric_limits<float>::max() 
			},
			{
			  std::numeric_limits<float>::lowest(),
			  std::numeric_limits<float>::lowest(),
			  std::numeric_limits<float>::lowest()
			}
		);
		AABB temp_box;	// to store intermediate results

		for (size_t i = 0; i < objects.GetCount(); i++)
		{
			objects[i].BoundingBox(t_start, t_end, temp_box);
			box = AABB::Combine(box, temp_box);
		}
	}


	// Builds Bounding Volume Hierarchy (BVH) structure for accelerating ray intersection tests.
	void BuildBVH(const float t_start, const float t_end) noexcept
	{
		if (!bvh_nodes.GetDataPtr())
		{
			bvh_nodes.Allocate(2 * objects.GetCount());
			BuildBVH(0, objects.GetCount(), BVH_ROOT_ID, t_start, t_end);
		}
	}


private:

	bool IntersectObjects(ObjectID first, size_t count, const Ray& ray, const float t_min, const float t_max, HitRecord& hit)
		const noexcept
	{
		bool any_hit = false;
		float t_closest = t_max;

		for (size_t i = 0; i < count; i++)
		{
			if (objects[first + i].Intersect(ray, t_min, t_closest, hit.t))
			{
				any_hit = true;
				t_closest = hit.t;
				hit.object_id = ObjectID(first + i);
			}
		}
		return any_hit;
	}


	bool IntersectBVH(NodeID node_id, const Ray& ray, const float t_min, const float t_max, HitRecord& hit)
		const noexcept
	{
		const NodeBVH& node = bvh_nodes[node_id];

		if (!node.bbox.Hit(ray, t_min, t_max))
			return false;

		if (node.IsLeaf())
		{
			return IntersectObjects(node.idx.first_object, node.count, ray, t_min, t_max, hit);
		}
		else
		{
			const bool hit_left = IntersectBVH(node.idx.left_child, ray, t_min, t_max, hit);
			const bool hit_right = IntersectBVH(node.idx.left_child + 1, ray, t_min, hit_left ? hit.t : t_max, hit);

			return hit_left || hit_right;
		}
	}


	void BuildBVH(size_t start, size_t end, NodeID node_id, float t_start, float t_end)
	{
		const size_t num_objects = end - start;

		if (num_objects == 1)
		{
			NodeBVH& node = bvh_nodes[node_id];
			objects[start].BoundingBox(t_start, t_end, node.bbox);
			node.idx.first_object = ObjectID(start);
			node.count = 1;
		}
		else if (num_objects == 2)
		{
			NodeBVH& node = bvh_nodes[node_id];
			AABB box_first, box_second;
			objects[start].BoundingBox(t_start, t_end, box_first);
			objects[end-1].BoundingBox(t_start, t_end, box_second);
			node.bbox = AABB::Combine(box_first, box_second);
			node.idx.first_object = ObjectID(start);
			node.count = 2;
		}
		else
		{
			const int axis = Random::GetInteger(0, 2);
			const auto comparator = [axis, t_start, t_end](const Hittable& a, const Hittable& b) -> bool
			{
				AABB box_a;
				AABB box_b;
				a.BoundingBox(t_start, t_end, box_a);
				b.BoundingBox(t_start, t_end, box_b);

				return box_a.min[axis] < box_b.min[axis];
			};

			std::sort(objects.GetDataPtr() + start, objects.GetDataPtr() + end, comparator);

			const NodeID left_child_id = ++bvh_node_id;
			const NodeID right_child_id = ++bvh_node_id;

			const size_t mid = start + (num_objects / 2);
			BuildBVH(start, mid, left_child_id, t_start, t_end);
			BuildBVH(mid, end, right_child_id, t_start, t_end);

			NodeBVH& node = bvh_nodes[node_id];
			node.bbox = AABB::Combine(bvh_nodes[left_child_id].bbox, bvh_nodes[right_child_id].bbox);
			node.idx.left_child = left_child_id;
			node.count = 0;
		}
	}
};
