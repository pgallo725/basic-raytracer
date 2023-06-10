#pragma once

#include <random>

#include "Vector3.h"


class Random
{
private:

	static thread_local std::mt19937_64 m_generator;

public:

	static void SeedCurrentThread(const unsigned long long seed) noexcept;

	static int     GetInteger(const int min, const int max) noexcept;
	static float   GetFloat(const float min, const float max) noexcept;
	static double  GetDouble(const double min, const double max) noexcept;
	static Vector3 GetVector(const float min, const float max) noexcept;
	static Color   GetColor(const float min = 0.0, const float max = 1.0) noexcept;
	
	static Vector3 GetUnitVector() noexcept;
	static Vector3 GetVectorInUnitSphere() noexcept;
	static Vector3 GetVectorInHemisphere(const Vector3& normal) noexcept;
	static Vector3 GetVectorInUnitDisk() noexcept;
};


class Perlin
{
public:

	Perlin();
	~Perlin();
	
	float Noise(const Point3& p) const noexcept;
	float TurbulentNoise(const Point3& p, int depth=7) const noexcept;

private:

	static const int c_nPoints = 256;
	Vector3* m_randomVectors;
	int*     m_permutationX;
	int*     m_permutationY;
	int*     m_permutationZ;

	static int* GeneratePermutation() noexcept;
	
	Vector3 GatherRandomSample(int i, int j, int k) const noexcept;
};
