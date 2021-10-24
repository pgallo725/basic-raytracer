#pragma once

#include <random>

#include "Vector3.h"


class Random
{
private:

	static thread_local std::mt19937_64 m_generator;

public:

	static void SeedCurrentThread(const unsigned long long seed) noexcept;

	static double  GetDouble(const double min, const double max) noexcept;
	static Vector3 GetVector(const double min, const double max) noexcept;
	static Color   GetColor(const double min = 0.0, const double max = 1.0) noexcept;
	
	static Vector3 GetUnitVector() noexcept;
	static Vector3 GetVectorInUnitSphere() noexcept;
	static Vector3 GetVectorInHemisphere(const Vector3& normal) noexcept;
	static Vector3 GetVectorInUnitDisk() noexcept;
};
