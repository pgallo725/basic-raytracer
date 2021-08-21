#pragma once

#include <cmath>
#include <iostream>

class Vector3 
{
public:

	double values[3];

public:

	Vector3() : values{ 0, 0, 0 } {};
	Vector3(double v1, double v2, double v3) : values{ v1, v2, v3 } {}

	double x() { return values[0]; }
	double y() { return values[1]; }
	double z() { return values[2]; }

	Vector3 operator-() { return Vector3(-values[0], -values[1], -values[2]); }
	double operator[](int i) const { return values[i]; }
	double& operator[](int i) { return values[i]; }

	Vector3& operator+=(const Vector3& other)
	{
		values[0] += other.values[0];
		values[1] += other.values[1];
		values[2] += other.values[2];
		return *this;
	}

	Vector3& operator*=(const double value)
	{
		values[0] *= value;
		values[1] *= value;
		values[2] *= value;
		return *this;
	}

	Vector3& operator/=(const double value)
	{
		return *this *= 1 / value;
	}

	double Length() const { return std::sqrt(SqrLength()); }
	double SqrLength() const { return values[0] * values[0] + values[1] * values[1] + values[2] * values[2]; }


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
		double length = vec.Length();
		return Vector3(vec.values[0] / length, vec.values[1] / length, vec.values[2] / length);
	}
};


inline std::ostream& operator<<(std::ostream& out, const Vector3& vec) 
{
	return out << vec.values[0] << ' ' << vec.values[1] << ' ' << vec.values[2];
}

inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(lhs.values[0] + rhs.values[0],
		lhs.values[1] + rhs.values[1],
		lhs.values[2] + rhs.values[2]);
}

inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(lhs.values[0] - rhs.values[0],
		lhs.values[1] - rhs.values[1],
		lhs.values[2] - rhs.values[2]);
}

inline Vector3 operator*(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3(lhs.values[0] * rhs.values[0],
		lhs.values[1] * rhs.values[1],
		lhs.values[2] * rhs.values[2]);
}

inline Vector3 operator*(const Vector3& vec, const double val)
{
	return Vector3(vec.values[0] * val,
		vec.values[1] * val,
		vec.values[2] * val);
}

inline Vector3 operator*(const double val, const Vector3& vec)
{
	return vec * val;
}

inline Vector3 operator/(const Vector3& vec, const double t)
{
	return vec * 1 / t;
}


// Type aliases for Vector3
using Point3 = Vector3;			// 3D point
using Color = Vector3;			// RGB color
