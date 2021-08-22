#pragma once

#include <cmath>
#include <iostream>

class Vector3 
{
public:

	double values[3];

public:

	Vector3() : values{ 0, 0, 0 } {}
	Vector3(double v1, double v2, double v3) : values{ v1, v2, v3 } {}

	inline double x() { return values[0]; }
	inline double y() { return values[1]; }
	inline double z() { return values[2]; }

	inline Vector3 operator-() const { return Vector3(-values[0], -values[1], -values[2]); }
	inline double operator[](int i) const { return values[i]; }
	inline double& operator[](int i) { return values[i]; }

	inline Vector3& operator+=(const Vector3& other)
	{
		values[0] += other.values[0];
		values[1] += other.values[1];
		values[2] += other.values[2];
		return *this;
	}

	inline Vector3& operator*=(const double value)
	{
		values[0] *= value;
		values[1] *= value;
		values[2] *= value;
		return *this;
	}

	inline Vector3& operator/=(const double value)
	{
		return *this *= 1 / value;
	}

	inline Vector3 operator+(const Vector3& other) const
	{
		return Vector3(this->values[0] + other.values[0],
			this->values[1] + other.values[1],
			this->values[2] + other.values[2]);
	}

	inline Vector3 operator-(const Vector3& other) const
	{
		return Vector3(this->values[0] - other.values[0],
			this->values[1] - other.values[1],
			this->values[2] - other.values[2]);
	}

	inline Vector3 operator*(const Vector3& other) const
	{
		return Vector3(this->values[0] * other.values[0],
			this->values[1] * other.values[1],
			this->values[2] * other.values[2]);
	}

	inline Vector3 operator*(const double val) const
	{
		return Vector3(this->values[0] * val,
			this->values[1] * val,
			this->values[2] * val);
	}

	inline Vector3 operator/(const double val) const
	{
		return *this * (1 / val);
	}


	inline double Length() const { return std::sqrt(SqrLength()); }
	inline double SqrLength() const { return values[0] * values[0] + values[1] * values[1] + values[2] * values[2]; }

	inline bool NearZero() const
	{
		// Return true if the vector is close to zero in all dimensions.
		const auto eps = 1e-8;
		return (fabs(values[0]) < eps) && (fabs(values[1]) < eps) && (fabs(values[2]) < eps);
	}


	inline static double Dot(const Vector3& a, const Vector3& b)
	{
		return a.values[0] * b.values[0] +
			a.values[1] * b.values[1] +
			a.values[2] * b.values[2];
	}

	inline static Vector3 Cross(const Vector3& a, const Vector3& b)
	{
		return Vector3(a.values[1] * b.values[2] - a.values[2] * b.values[1],
			a.values[2] * b.values[0] - a.values[0] * b.values[2],
			a.values[0] * b.values[1] - a.values[1] * b.values[0]);
	}

	inline static Vector3 Normalized(const Vector3& vec)
	{
		return vec / vec.Length();
	}

	inline static Vector3 Reflect(const Vector3& vec, const Vector3& normal)
	{
		// Order of operands changed due to operator* shenanigans
		// it should be r = v - 2 * (v.n) * n
		return vec - normal * 2 * Vector3::Dot(vec, normal);
	}

	inline static Vector3 Refract(const Vector3& vec, const Vector3& normal, double etai_over_etat)
	{
		// Splitting the refracted ray into a R'_perpendicular and R'_parallel,
		// using Snell's law we derive that R'_perp = etai/etat * (R + cos(theta)*n)
		// by exploiting the definition of dot product and restricting the vectors
		// to be of unit length, it can be written as R'_perp = etai/etat * (R + (-R.n)*n)
		double cos_theta = fmin(Vector3::Dot(-vec, normal), 1.0);

		Vector3 r_perpendicular = (vec + normal * cos_theta) * etai_over_etat;

		// We also derive that R'_parallel = -sqrt(1 - |R'_perp|^2) * n
		Vector3 r_parallel = normal * -std::sqrt(fabs(1.0 - r_perpendicular.SqrLength()));

		return r_parallel + r_perpendicular;
	}
};


inline std::ostream& operator<<(std::ostream& out, const Vector3& vec) 
{
	return out << vec.values[0] << ' ' << vec.values[1] << ' ' << vec.values[2];
}

inline Vector3 operator*(const double val, const Vector3& vec)
{
	return vec * val;
}


// Type aliases for Vector3
using Point3 = Vector3;			// 3D point
using Color = Vector3;			// RGB color
