#pragma once

#include "Common.h"
#include "Texture.h"


// Defines an abstract common interface for all materials.
class Material
{
public:

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered) 
		const noexcept = 0;
};


class LambertianColor : public Material
{
public:

	const Color albedo;

public:

	LambertianColor(const Color& color) noexcept
		: albedo(color) {}

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered)
		const noexcept override final
	{
		// Scatter the incoming ray in a random direction off the surface
		// (offset by the face normal to avoid rays going inside the surface).
		Vector3 scatter_direction = hit.normal + Random::GetUnitVector();

		// Catch potentially degenerate scatter direction.
		if (scatter_direction.NearZero())
			scatter_direction = hit.normal;

		ray_scattered = Ray(hit.point, scatter_direction, ray_in.time);
		attenuation = albedo;
		return true;
	}
};


class LambertianTexture : public Material
{
public:

	const std::shared_ptr<Texture> albedo;

public:

	LambertianTexture(const std::shared_ptr<Texture>& texture) noexcept
		: albedo(texture) {}

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered)
		const noexcept override final
	{
		// Scatter the incoming ray in a random direction off the surface
		// (offset by the face normal to avoid rays going inside the surface).
		Vector3 scatter_direction = hit.normal + Random::GetUnitVector();

		// Catch potentially degenerate scatter direction.
		if (scatter_direction.NearZero())
			scatter_direction = hit.normal;

		ray_scattered = Ray(hit.point, scatter_direction, ray_in.time);
		attenuation = albedo->Sample(hit.u, hit.v, hit.point);
		return true;
	}
};


class Metal : public Material
{
public:

	const Color albedo;
	const double fuzz;

public:

	Metal(const Color& color, const double fuzz) noexcept
		: albedo(color), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered) 
		const noexcept override final
	{
		// Metallic reflection of the incoming ray along the surface normal.
		const Vector3 unit_direction = Vector3::Normalized(ray_in.direction);
		const Vector3 reflected = Vector3::Reflect(unit_direction, hit.normal);

		// Adding fuzziness to the reflected ray by slightly changing the ray direction.
		ray_scattered = Ray(hit.point, reflected + fuzz * Random::GetVectorInUnitSphere(), ray_in.time);
		attenuation = albedo;
		return (Vector3::Dot(ray_scattered.direction, hit.normal) > 0.0);
	}
};


class Dielectric : public Material
{
public:

	const double ir;	// Index of Refraction

public:

	Dielectric(const double index_of_refraction) noexcept
		: ir(index_of_refraction) {}

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered) 
		const noexcept override final
	{
		const Vector3 unit_direction = Vector3::Normalized(ray_in.direction);

		// Calculate the ratio between indexes of refraction (air = 1.0)
		const double refraction_ratio = hit.is_front_face ? (1.0 / ir) : ir;

		// Using Snell's law to determine whether the incoming ray
		// can be refracted or only reflected (Total Internal Reflection).
		const double cos_theta = fmin(Vector3::Dot(-unit_direction, hit.normal), 1.0);
		const double sin_theta = std::sqrt(1 - cos_theta * cos_theta);

		const bool reflect = CannotRefract(sin_theta, refraction_ratio) || ShouldReflect(cos_theta, refraction_ratio);

		Vector3 out_direction = reflect ? 
			Vector3::Reflect(unit_direction, hit.normal) :
			Vector3::Refract(unit_direction, hit.normal, refraction_ratio);

		ray_scattered = Ray(hit.point, out_direction, ray_in.time);
		attenuation = Color(1.0, 1.0, 1.0);
		return true;
	}

private:

	inline static bool CannotRefract(const double sin_theta, const double refraction_ratio) noexcept
	{
		return refraction_ratio * sin_theta > 1.0;
	}

	inline static bool ShouldReflect(const double cos_theta, const double refraction_ratio) noexcept
	{
		return Reflectance(cos_theta, refraction_ratio) > Random::GetDouble(0.0, 1.0);
	}

	inline static double Reflectance(const double cosine, const double refraction) noexcept
	{
		// Use Schlick's approximation for reflectance.
		// (a.k.a. varying reflectivity based on the angle)
		auto r0 = (1 - refraction) / (1 + refraction);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};
