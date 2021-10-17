#pragma once

#include <cmath>
#include <iostream>

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;


class Vector3 
{
public:

	double values[3];

public:

	Vector3() : values{ 0, 0, 0 } {}
	Vector3(double v1, double v2, double v3) : values{ v1, v2, v3 } {}

	inline double x() const { return values[0]; }
	inline double y() const { return values[1]; }
	inline double z() const { return values[2]; }

	inline double operator[](int i) const { return values[i]; }
	inline double& operator[](int i) { return values[i]; }

	Vector3 operator-() const;

	Vector3& operator+=(const Vector3& other);
	Vector3& operator*=(const double value);
	Vector3& operator/=(const double value);

	Vector3 operator+(const Vector3& other) const;
	Vector3 operator-(const Vector3& other) const;
	Vector3 operator*(const Vector3& other) const;
	Vector3 operator*(const double val) const;
	Vector3 operator/(const double val) const;

	double Length() const;
	double SqrLength() const;

	bool NearZero() const;

	static Vector3 Normalized(const Vector3& vec);

	static double Dot(const Vector3& a, const Vector3& b);
	static Vector3 Cross(const Vector3& a, const Vector3& b);

	static Vector3 Reflect(const Vector3& vec, const Vector3& normal);
	static Vector3 Refract(const Vector3& vec, const Vector3& normal, double etai_over_etat);
};


// Non-member Vector3 operators
inline std::ostream& operator<<(std::ostream& out, const Vector3& vec);
inline Vector3 operator*(const double val, const Vector3& vec);


// Type aliases for Vector3
using Point3 = Vector3;			// 3D point
using Color = Vector3;			// RGB color


// JSON de/serialization functions
void to_json(json& j, const Vector3& vec);
void from_json(const json& j, Vector3& vec);
