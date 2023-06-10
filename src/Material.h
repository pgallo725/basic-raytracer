#pragma once

#include "Common.h"
#include "Texture.h"


struct LambertianColor
{
	Color albedo;

	bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered)
		const noexcept
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

struct LambertianTexture
{
	std::shared_ptr<Texture> texture;

	bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered)
		const noexcept
	{
		// Scatter the incoming ray in a random direction off the surface
		// (offset by the face normal to avoid rays going inside the surface).
		Vector3 scatter_direction = hit.normal + Random::GetUnitVector();

		// Catch potentially degenerate scatter direction.
		if (scatter_direction.NearZero())
			scatter_direction = hit.normal;

		ray_scattered = Ray(hit.point, scatter_direction, ray_in.time);
		attenuation = texture->Sample(hit.u, hit.v, hit.point);
		return true;
	}
};


struct Metal
{
	Color albedo;
	float fuzz;		// TODO: must be clamped to be < 1.0

	bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered) 
		const noexcept
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


struct Dielectric
{
	float ir;	// Index of Refraction

	bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered) 
		const noexcept
	{
		const Vector3 unit_direction = Vector3::Normalized(ray_in.direction);

		// Calculate the ratio between indexes of refraction (air = 1.0)
		const float refraction_ratio = hit.is_front_face ? (1.0 / ir) : ir;

		// Using Snell's law to determine whether the incoming ray
		// can be refracted or only reflected (Total Internal Reflection).
		const float cos_theta = fmin(Vector3::Dot(-unit_direction, hit.normal), 1.0);
		const float sin_theta = std::sqrt(1 - cos_theta * cos_theta);

		const bool reflect = CannotRefract(sin_theta, refraction_ratio) || ShouldReflect(cos_theta, refraction_ratio);

		Vector3 out_direction = reflect ? 
			Vector3::Reflect(unit_direction, hit.normal) :
			Vector3::Refract(unit_direction, hit.normal, refraction_ratio);

		ray_scattered = Ray(hit.point, out_direction, ray_in.time);
		attenuation = Color(1.0, 1.0, 1.0);
		return true;
	}

private:

	inline static bool CannotRefract(const float sin_theta, const float refraction_ratio) noexcept
	{
		return refraction_ratio * sin_theta > 1.0;
	}

	inline static bool ShouldReflect(const float cos_theta, const float refraction_ratio) noexcept
	{
		return Reflectance(cos_theta, refraction_ratio) > Random::GetFloat(0.0, 1.0);
	}

	inline static float Reflectance(const float cosine, const float refraction) noexcept
	{
		// Use Schlick's approximation for reflectance.
		// (a.k.a. varying reflectivity based on the angle)
		float r0 = (1.0 - refraction) / (1.0 + refraction);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};


struct DiffuseLight
{
	Color color;
};


struct Isotropic
{
	Color color;

	bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered)
		const noexcept
	{
		// An isotropic material's scattering function picks a uniformly random direction
		ray_scattered = Ray(hit.point, Random::GetVectorInUnitSphere(), ray_in.time);
		attenuation = color;
		return true;
	}
};


//*************************************************************
// Defines a single data structure that can hold any material.
struct Material
{
	// NOTE: values of this enum must match with the
	//  index of the corresponding type in MaterialData.
	enum class Type
	{
		LambertianColor   = 0,
		LambertianTexture = 1,
		Metal             = 2,
		Dielectric        = 3,
		DiffuseLight      = 4,
		Isotropic         = 5
	};

	using MaterialData = std::variant
	<
		LambertianColor,			// 12 bytes
		LambertianTexture,			//  8 bytes
		Metal,						// 16 bytes
		Dielectric,					//  4 bytes
		DiffuseLight,				// 12 bytes
		Isotropic					// 12 bytes
	>;								//----------
	MaterialData data;				// 24 bytes (16 bytes + std::variant<> overhead/alignment)


	Color Emitted([[maybe_unused]] const Ray& ray_in, [[maybe_unused]] const HitRecord& hit)
		const noexcept
	{
		if (std::holds_alternative<DiffuseLight>(data))
			return std::get<DiffuseLight>(data).color;
		else return Color{ 0, 0, 0 };
	}

	bool Scatter(const Ray& ray_in, const HitRecord& hit, Color& attenuation, Ray& ray_scattered)
		const noexcept
	{
		switch (Material::Type(data.index()))
		{
			case Type::LambertianColor:   return std::get<LambertianColor>(data).Scatter(ray_in, hit, attenuation, ray_scattered);
			case Type::LambertianTexture: return std::get<LambertianTexture>(data).Scatter(ray_in, hit, attenuation, ray_scattered);
			case Type::Metal:             return std::get<Metal>(data).Scatter(ray_in, hit, attenuation, ray_scattered);
			case Type::Dielectric:        return std::get<Dielectric>(data).Scatter(ray_in, hit, attenuation, ray_scattered);
			case Type::Isotropic:         return std::get<Isotropic>(data).Scatter(ray_in, hit, attenuation, ray_scattered);
			default:                      return false;
		}
	}


	static void CreateLambertianColor(Material& material, const Color& color) noexcept
	{
		material.data = LambertianColor{ .albedo = color };
	}

	static void CreateLambertianTexture(Material& material, const std::shared_ptr<Texture>& texture) noexcept
	{
		material.data = LambertianTexture{ .texture = texture };
	}

	static void CreateMetal(Material& material, const Color& color, float fuzz) noexcept
	{
		material.data = Metal{ .albedo = color, .fuzz = std::min(fuzz, 1.0f) };
	}

	static void CreateDielectric(Material& material, const float ior) noexcept
	{
		material.data = Dielectric{ .ir = ior };
	}

	static void CreateDiffuseLight(Material& material, const Color& color) noexcept
	{
		material.data = DiffuseLight{ .color = color };
	}

	static void CreateIsotropic(Material& material, const Color& color) noexcept
	{
		material.data = Isotropic{ .color = color };
	}

	static Type GetTypeFromString(const std::string& str)
	{
		static std::map<std::string, Type> str_type_map
		{
			{ "LambertianColor"  , Type::LambertianColor   },
			{ "LambertianTexture", Type::LambertianTexture },
			{ "Metal"            , Type::Metal             },
			{ "Dielectric"       , Type::Dielectric        },
			{ "DiffuseLight"     , Type::DiffuseLight      },
			{ "Isotropic"        , Type::Isotropic         },
		};
		return str_type_map.at(str);
	}
};
