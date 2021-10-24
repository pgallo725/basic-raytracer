#pragma once

class Vector3;
using Point3 = Vector3;			// 3D point
using Color = Vector3;			// RGB color


class Vector3 
{
public:

	double values[3];

public:

	Vector3() noexcept : values{ 0, 0, 0 } {}
	Vector3(double v1, double v2, double v3) noexcept : values{ v1, v2, v3 } {}

	double x() const noexcept { return values[0]; }
	double y() const noexcept { return values[1]; }
	double z() const noexcept { return values[2]; }

	double operator[](const int i) const noexcept { return values[i]; }
	double& operator[](const int i) noexcept { return values[i]; }

	Vector3 operator-() const noexcept;

	Vector3& operator+=(const Vector3& other) noexcept;
	Vector3& operator*=(const double value) noexcept;
	Vector3& operator/=(const double value) noexcept;

	Vector3 operator+(const Vector3& other) const noexcept;
	Vector3 operator-(const Vector3& other) const noexcept;
	Vector3 operator*(const Vector3& other) const noexcept;
	Vector3 operator*(const double val) const noexcept;
	Vector3 operator/(const double val) const noexcept;

	friend Vector3 operator*(const double val, const Vector3& vec) noexcept;

	double Length() const noexcept;
	double SqrLength() const noexcept;

	bool NearZero() const noexcept;

	static Vector3 Normalized(const Vector3& vec) noexcept;

	static double Dot(const Vector3& a, const Vector3& b) noexcept;
	static Vector3 Cross(const Vector3& a, const Vector3& b) noexcept;

	static Vector3 Reflect(const Vector3& vec, const Vector3& normal) noexcept;
	static Vector3 Refract(const Vector3& vec, const Vector3& normal, const double etai_over_etat) noexcept;
};
