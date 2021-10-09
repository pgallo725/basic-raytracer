#pragma once

#include <random>

#include "Vector3.h"


class Random
{
private:

	static thread_local std::mt19937_64 m_generator;

public:

	inline static void SeedCurrentThread(unsigned long long seed)
	{
		m_generator.seed(seed);
	}

	inline static double GetDouble(const double min, const double max)
	{
		std::uniform_real_distribution<double> distribution(min, max);
		return distribution(m_generator);
	}

	inline static Vector3 GetVector(const double min, const double max)
	{
		return Vector3(Random::GetDouble(min, max),
			Random::GetDouble(min, max),
			Random::GetDouble(min, max));
	}

	inline static Vector3 GetUnitVector()
	{
		return Vector3::Normalized(Random::GetVectorInUnitSphere());
	}

	inline static Vector3 GetVectorInUnitSphere()
	{
		while (true)
		{
			// Pick a random point in the unit cube, where x, y, and z in [-1, +1].
			Vector3 vec = Random::GetVector(-1, 1);
			// Reject this point and try again if the point is outside the sphere.
			if (vec.SqrLength() >= 1) continue;
			return vec;
		}
	}

	inline static Vector3 GetVectorInHemisphere(const Vector3& normal)
	{
		Vector3 in_unit_sphere = Random::GetVectorInUnitSphere();
		return Vector3::Dot(in_unit_sphere, normal) > 0.0 ?			// In the same hemisphere as the normal
			in_unit_sphere : -in_unit_sphere;
	}

	inline static Vector3 GetVectorInUnitDisk()
	{
		while (true)
		{
			Vector3 vec = Vector3(Random::GetDouble(-1, 1), Random::GetDouble(-1, 1), 0);
			if (vec.SqrLength() >= 1.0) continue;
			return vec;
		}
	}

	inline static Color GetColor(const double min = 0.0, const double max = 1.0)
	{
		return Random::GetVector(min, max);
	}
};
