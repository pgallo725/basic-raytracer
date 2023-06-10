#pragma once

class Vector3;
using Point3 = Vector3;			// 3D point
using Color = Vector3;			// RGB color


class Vector3 
{
public:

	float values[3];

public:

	Vector3() noexcept : values{ 0, 0, 0 } {}
	Vector3(float v1, float v2, float v3) noexcept : values{ v1, v2, v3 } {}

	float x() const noexcept { return values[0]; }
	float y() const noexcept { return values[1]; }
	float z() const noexcept { return values[2]; }

	float operator[](const int i) const noexcept { return values[i]; }
	float& operator[](const int i) noexcept { return values[i]; }

	Vector3 operator-() const noexcept;

	Vector3& operator+=(const Vector3& other) noexcept;
	Vector3& operator*=(const float value) noexcept;
	Vector3& operator/=(const float value) noexcept;

	Vector3 operator+(const Vector3& other) const noexcept;
	Vector3 operator-(const Vector3& other) const noexcept;
	Vector3 operator*(const Vector3& other) const noexcept;
	Vector3 operator/(const Vector3& other) const noexcept;
	Vector3 operator*(const float val) const noexcept;
	Vector3 operator/(const float val) const noexcept;

	friend Vector3 operator*(const float val, const Vector3& vec) noexcept;
	friend Vector3 operator/(const float val, const Vector3& vec) noexcept;

	float Length() const noexcept;
	float SqrLength() const noexcept;

	bool NearZero() const noexcept;

	static Vector3 Normalized(const Vector3& vec) noexcept;

	static float Dot(const Vector3& a, const Vector3& b) noexcept;
	static Vector3 Cross(const Vector3& a, const Vector3& b) noexcept;

	static Vector3 Reflect(const Vector3& vec, const Vector3& normal) noexcept;
	static Vector3 Refract(const Vector3& vec, const Vector3& normal, const float etai_over_etat) noexcept;
};
