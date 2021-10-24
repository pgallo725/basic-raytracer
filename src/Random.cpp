#include "Random.h"
#include "Common.h"


// Initialize the random number generator
thread_local std::mt19937_64 Random::m_generator = std::mt19937_64();


void Random::SeedCurrentThread(const unsigned long long seed) noexcept
{
	m_generator.seed(seed);
}


double Random::GetDouble(const double min, const double max) noexcept
{
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(m_generator);
}

Vector3 Random::GetVector(const double min, const double max) noexcept
{
	return Vector3(Random::GetDouble(min, max),
		Random::GetDouble(min, max),
		Random::GetDouble(min, max));
}

Color Random::GetColor(const double min, const double max) noexcept
{
	return Random::GetVector(Clamp(min, 0.0, 1.0), Clamp(max, 0.0, 1.0));
}


Vector3 Random::GetUnitVector() noexcept
{
	return Vector3::Normalized(Random::GetVectorInUnitSphere());
}

Vector3 Random::GetVectorInUnitSphere() noexcept
{
	while (true)
	{
		// Pick a random point in the unit cube, where x, y, and z in [-1, +1].
		const Vector3 vec = Random::GetVector(-1, 1);
		// Reject this point and try again if the point is outside the sphere.
		if (vec.SqrLength() >= 1) continue;
		return vec;
	}
}

Vector3 Random::GetVectorInHemisphere(const Vector3& normal) noexcept
{
	const Vector3 in_unit_sphere = Random::GetVectorInUnitSphere();
	// Make sure it's in the same hemisphere as the normal.
	return Vector3::Dot(in_unit_sphere, normal) > 0.0 ?
		in_unit_sphere : -in_unit_sphere;
}

Vector3 Random::GetVectorInUnitDisk() noexcept
{
	while (true)
	{
		const Vector3 vec = Vector3(Random::GetDouble(-1, 1), Random::GetDouble(-1, 1), 0);
		if (vec.SqrLength() >= 1.0) continue;
		return vec;
	}
}
