#pragma once

#include "Common.h"
#include "Hittable.h"
#include "Material.h"
#include "Texture.h"


class ConstantMedium : public Hittable
{
public:

	double neg_inv_density = 0.0;
	std::shared_ptr<Hittable> boundary;
	std::shared_ptr<Material> phase_function;

public:

	ConstantMedium(std::shared_ptr<Hittable> boundary, const double density, const Color& color)
		: neg_inv_density(-1.0 / density)
		, boundary(boundary)
		, phase_function(std::make_shared<Isotropic>(color))
	{}


	// Ray-Volume (convex) intersection checking.
	virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit)
		const noexcept override final
	{
		// A ray passing through a volume of constant density can either scatter inside
		// the volume or make it all the way through. The denser the volume, the more
		// likely it is for the ray to be scattered; the probability of the ray scattering
		// over a small distance is P = C * dL, where C is proportional to the optical
		// density of the volume.
		// The code below (given a random number) calculates the distance at which the 
		// scattering of a ray would occur: if that distance is inside the volume boundary
		// it's a hit, otherwise means that there is no "hit".

		HitRecord hit1, hit2;

		if (!boundary->Hit(ray, -Infinity, Infinity, hit1))
			return false;

		if (!boundary->Hit(ray, hit1.t + 0.0001, Infinity, hit2))
			return false;

		hit1.t = std::max(hit1.t, t_min);
		hit2.t = std::min(hit2.t, t_max);

		if (hit1.t >= hit2.t)
			return false;

		if (hit1.t < 0.0)
			hit1.t = 0.0;

		const double ray_length = ray.direction.Length();
		const double distance_inside_boundary = (hit2.t - hit1.t) * ray_length;
		const double hit_distance = neg_inv_density * std::log(Random::GetDouble(0.0, 1.0));

		if (hit_distance > distance_inside_boundary)
			return false;

		hit.t = hit1.t + hit_distance / ray_length;
		hit.point = ray.At(hit.t);
		hit.normal = Vector3(1, 0, 0);			// arbitrary
		hit.is_front_face = true;				// also arbitrary
		hit.material = phase_function.get();
		return true;
	}


	// Volume bounding box.
	virtual bool BoundingBox(const double t_start, const double t_end, AABB& box)
		const noexcept override final
	{
		return boundary->BoundingBox(t_start, t_end, box);
	}
};