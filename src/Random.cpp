#include "Random.h"
#include "Common.h"


// Initialize the random number generator
thread_local std::mt19937_64 Random::m_generator = std::mt19937_64();


void Random::SeedCurrentThread(const unsigned long long seed) noexcept
{
	m_generator.seed(seed);
}


int Random::GetInteger(const int min, const int max) noexcept
{
	std::uniform_real_distribution<float> distribution(min, max+1);
	return static_cast<int>(distribution(m_generator));
}

float Random::GetFloat(const float min, const float max) noexcept
{
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(m_generator);
}

double Random::GetDouble(const double min, const double max) noexcept
{
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(m_generator);
}

Vector3 Random::GetVector(const float min, const float max) noexcept
{
	return Vector3(Random::GetFloat(min, max),
		Random::GetFloat(min, max),
		Random::GetFloat(min, max));
}

Color Random::GetColor(const float min, const float max) noexcept
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
		const Vector3 vec = Vector3(Random::GetFloat(-1, 1), Random::GetFloat(-1, 1), 0);
		if (vec.SqrLength() >= 1.0) continue;
		return vec;
	}
}


Perlin::Perlin()
{
	m_randomVectors = new Vector3[c_nPoints];
	for (int i = 0; i < c_nPoints; ++i)
		m_randomVectors[i] = Random::GetUnitVector();

	m_permutationX = GeneratePermutation();
	m_permutationY = GeneratePermutation();
	m_permutationZ = GeneratePermutation();
}

Perlin::~Perlin()
{
	delete[] m_randomVectors;
	delete[] m_permutationX;
	delete[] m_permutationY;
	delete[] m_permutationZ;
}

float Perlin::Noise(const Point3& p) const noexcept
{
	const int x = static_cast<int>(std::floor(p.x()));
	const int y = static_cast<int>(std::floor(p.y()));
	const int z = static_cast<int>(std::floor(p.z()));

	// Gather 8 noise samples
	Vector3 c[2][2][2];
	c[0][0][0] = GatherRandomSample(x,   y,   z  );
	c[1][0][0] = GatherRandomSample(x+1, y,   z  );
	c[0][1][0] = GatherRandomSample(x,   y+1, z  );
	c[1][1][0] = GatherRandomSample(x+1, y+1, z  );
	c[0][0][1] = GatherRandomSample(x,   y,   z+1);
	c[1][0][1] = GatherRandomSample(x+1, y,   z+1);
	c[0][1][1] = GatherRandomSample(x,   y+1, z+1);
	c[1][1][1] = GatherRandomSample(x+1, y+1, z+1);

	const float u = p.x() - std::floor(p.x());
	const float v = p.y() - std::floor(p.y());
	const float w = p.z() - std::floor(p.z());

	// Use Hermite cubic function to smooth the interpolation factors
	const float uu = u * u * (3.0 - 2.0 * u);
	const float vv = v * v * (3.0 - 2.0 * v);
	const float ww = w * w * (3.0 - 2.0 * w);

	// Trilinearly interpolate 8 noise samples to smooth out the result
	float result = 0.0;
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				Vector3 weight(u - i, v - j, w - k);
				result += (i * uu + (1.0 - i) * (1.0 - uu))
						* (j * vv + (1.0 - j) * (1.0 - vv))
						* (k * ww + (1.0 - k) * (1.0 - ww))
						* Vector3::Dot(c[i][j][k], weight);
			}
	return result;
}

float Perlin::TurbulentNoise(const Point3& p, int depth) const noexcept
{
	float result = 0.0;
	float scale = 1.0;
	float weight = 1.0;

	for (int i = 0; i < depth; ++i)
	{
		result += weight * Noise(p * scale);
		scale *= 2.0;
		weight *= 0.5;
	}

	return std::fabs(result);
}

int* Perlin::GeneratePermutation() noexcept
{
	int* p = new int[c_nPoints];

	for (int i = 0; i < c_nPoints; ++i)
		p[i] = i;

	for (int i = c_nPoints - 1; i > 0; --i)
	{
		const int target = Random::GetInteger(0, i);
		std::swap(p[i], p[target]);
	}

	return p;
}

Vector3 Perlin::GatherRandomSample(int i, int j, int k) const noexcept
{
	return m_randomVectors
	[
		m_permutationX[i & 255] ^
		m_permutationY[j & 255] ^
		m_permutationZ[k & 255]
	];
}
