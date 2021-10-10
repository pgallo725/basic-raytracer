#pragma once

#include "Common.h"

struct HitRecord;	// Forward declaration


class Material
{
public:

	static std::shared_ptr<Material> Deserialize(const json& j);

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit,
		Color& attenuation, Ray& ray_scattered) const = 0;
};


class Lambertian : public Material
{
public:

	Color albedo;

public:

	Lambertian(const Color& color) : albedo(color) {}

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit,
		Color& attenuation, Ray& ray_scattered) const override
	{
		// Scatter the incoming ray in a random direction off the surface
		// (offset by the face normal to avoid rays going inside the surface).
		Vector3 scatter_direction = hit.normal + Random::GetUnitVector();

		// Catch potentially degenerate scatter direction.
		if (scatter_direction.NearZero())
			scatter_direction = hit.normal;

		ray_scattered = Ray(hit.point, scatter_direction);
		attenuation = albedo;
		return true;
	}
};


class Metal : public Material
{
public:

	Color albedo;
	double fuzz;

public:

	Metal(const Color& color, double fuzz) : albedo(color), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit,
		Color& attenuation, Ray& ray_scattered) const override
	{
		// Metallic reflection of the incoming ray along the surface normal.
		Vector3 unit_direction = Vector3::Normalized(ray_in.direction);
		Vector3 reflected = Vector3::Reflect(unit_direction, hit.normal);
		// Adding fuzziness to the reflected ray by slightly changing the ray direction.
		ray_scattered = Ray(hit.point, reflected + fuzz * Random::GetVectorInUnitSphere());
		attenuation = albedo;
		return (Vector3::Dot(ray_scattered.direction, hit.normal) > 0.0);
	}
};


class Dielectric : public Material
{
public:

	double ir;	// Index of Refraction

public:

	Dielectric(const double index_of_refraction) : ir(index_of_refraction) {}

	virtual bool Scatter(const Ray& ray_in, const HitRecord& hit,
		Color& attenuation, Ray& ray_scattered) const override
	{
		attenuation = Color(1.0, 1.0, 1.0);

		// Calculate the ratio between indexes of refraction (air = 1.0)
		double refraction_ratio = hit.is_front_face ? (1.0 / ir) : ir;

		Vector3 unit_direction = Vector3::Normalized(ray_in.direction);

		// Using Snell's law to determine whether the incoming ray
		// can be refracted or only reflected (Total Internal Reflection).
		double cos_theta = fmin(Vector3::Dot(-unit_direction, hit.normal), 1.0);
		double sin_theta = std::sqrt(1 - cos_theta * cos_theta);
		bool cannot_refract = refraction_ratio * sin_theta > 1.0;

		Vector3 out_direction;
		if (cannot_refract || Reflectance(cos_theta, refraction_ratio) > Random::GetDouble(0.0, 1.0))
			out_direction = Vector3::Reflect(unit_direction, hit.normal);
		else out_direction = Vector3::Refract(unit_direction, hit.normal, refraction_ratio);

		ray_scattered = Ray(hit.point, out_direction);
		return true;
	}

private:

	inline static double Reflectance(double cosine, double refraction_index)
	{
		// Use Schlick's approximation for reflectance.
		// (a.k.a. varying reflectivity based on the angle)
		auto r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};



std::shared_ptr<Material> Material::Deserialize(const json& j)
{
	std::string type = j.at("type").get<std::string>();

	if (type.compare("Lambertian") == 0)
	{
		return std::make_shared<Lambertian>(j.at("albedo").get<Color>());
	}
	else if (type.compare("Metal") == 0)
	{
		return std::make_shared<Metal>(j.at("albedo").get<Color>(), j.at("fuzz").get<double>());
	}
	else if (type.compare("Dielectric") == 0)
	{
		return std::make_shared<Dielectric>(j.at("ior").get<double>());
	}

	throw std::exception(("Invalid material type: " + type).c_str());
}
