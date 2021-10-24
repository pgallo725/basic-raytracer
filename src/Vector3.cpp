#include "Vector3.h"
#include "Common.h"


Vector3 Vector3::operator-() const noexcept
{
	return Vector3(-values[0], -values[1], -values[2]);
}

Vector3& Vector3::operator+=(const Vector3& other) noexcept
{
	values[0] += other.values[0];
	values[1] += other.values[1];
	values[2] += other.values[2];
	return *this;
}

Vector3& Vector3::operator*=(const double value) noexcept
{
	values[0] *= value;
	values[1] *= value;
	values[2] *= value;
	return *this;
}

Vector3& Vector3::operator/=(const double value) noexcept
{
	return *this *= 1.0 / value;
}

Vector3 Vector3::operator+(const Vector3& other) const noexcept
{
	return Vector3(this->values[0] + other.values[0],
		this->values[1] + other.values[1],
		this->values[2] + other.values[2]);
}

Vector3 Vector3::operator-(const Vector3& other) const noexcept
{
	return Vector3(this->values[0] - other.values[0],
		this->values[1] - other.values[1],
		this->values[2] - other.values[2]);
}

Vector3 Vector3::operator*(const Vector3& other) const noexcept
{
	return Vector3(this->values[0] * other.values[0],
		this->values[1] * other.values[1],
		this->values[2] * other.values[2]);
}

Vector3 Vector3::operator*(const double val) const noexcept
{
	return Vector3(this->values[0] * val,
		this->values[1] * val,
		this->values[2] * val);
}

Vector3 operator*(const double val, const Vector3& vec) noexcept
{
	return vec * val;
}

Vector3 Vector3::operator/(const double val) const noexcept
{
	return *this * (1.0 / val);
}

double Vector3::Length() const noexcept
{
	return std::sqrt(SqrLength());
}

double Vector3::SqrLength() const noexcept
{
	return values[0] * values[0] + values[1] * values[1] + values[2] * values[2];
}

bool Vector3::NearZero() const noexcept
{
	// Return true if the vector is close to zero in all dimensions.
	const auto eps = 1e-8;
	return (fabs(values[0]) < eps) && (fabs(values[1]) < eps) && (fabs(values[2]) < eps);
}

double Vector3::Dot(const Vector3& a, const Vector3& b) noexcept
{
	return a.values[0] * b.values[0] +
		a.values[1] * b.values[1] +
		a.values[2] * b.values[2];
}

Vector3 Vector3::Cross(const Vector3& a, const Vector3& b) noexcept
{
	return Vector3(a.values[1] * b.values[2] - a.values[2] * b.values[1],
		a.values[2] * b.values[0] - a.values[0] * b.values[2],
		a.values[0] * b.values[1] - a.values[1] * b.values[0]);
}

Vector3 Vector3::Normalized(const Vector3& vec) noexcept
{
	return vec / vec.Length();
}

Vector3 Vector3::Reflect(const Vector3& vec, const Vector3& normal) noexcept
{
	// r = v - 2 * (v.n) * n
	return vec - 2 * Vector3::Dot(vec, normal) * normal;
}

Vector3 Vector3::Refract(const Vector3& vec, const Vector3& normal, const double etai_over_etat) noexcept
{
	// Splitting the refracted ray into a R'_perpendicular and R'_parallel,
	// using Snell's law we derive that R'_perp = etai/etat * (R + cos(theta)*n)
	// by exploiting the definition of dot product and restricting the vectors
	// to be of unit length, it can be written as R'_perp = etai/etat * (R + (-R.n)*n)
	double cos_theta = fmin(Vector3::Dot(-vec, normal), 1.0);

	Vector3 r_perpendicular = etai_over_etat * (vec + cos_theta * normal);

	// We also derive that R'_parallel = -sqrt(1 - |R'_perp|^2) * n
	Vector3 r_parallel = -std::sqrt(fabs(1.0 - r_perpendicular.SqrLength())) * normal;

	return r_parallel + r_perpendicular;
}
