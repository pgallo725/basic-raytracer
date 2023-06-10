#pragma once

#include <cassert>

#include "Common.h"
#include "AABB.h"


struct Sphere
{
	Point3 center;
	float  radius;


	bool Intersect(const Ray& ray, const float t_min, const float t_max, float& t_hit)
		const noexcept
	{
		// Check if there exists any 't' that defines a point P which satisfies
		// the equation (Px - Cx)^2 + (Py - Cy)^2 + (Pz - Cz)^2 = r^2,
		// rewritten as ((O + t * d) - C).((O + t * d) - C) - r^2 = 0,
		// where O is the ray origin and d is the ray direction,
		// which defines a point on the sphere's surface (an intersection)
		// The equation is unrolled as t^2 * d.d + + 2 * t * d.(O - C) + (O - C).(O - C) - r^2 = 0
		// Solving for the unknown t, this means that:
		//  a = |d|^2
		//  b = 2 * d.(O - C)
		//  c = |O - C|^2 - r^2
		// Using h = d.(O - C) such as 2h = b it is possible to simplify the calculation of
		// the discriminant and the solutions of the equation by removing some multiplications.
		const Vector3 oc = ray.origin - center;
		const float a = ray.direction.SqrLength();
		const float h = Vector3::Dot(oc, ray.direction);
		const float c = oc.SqrLength() - radius * radius;
		const float discriminant = h * h - a * c;

		if (discriminant < 0) return false;
		const float sqrtd = std::sqrt(discriminant);

		// Find the nearest root that lies in the specified range.
		float root = (-h - sqrtd) / a;
		if ((root < t_min) | (root > t_max))
		{
			root = (-h + sqrtd) / a;
			if ((root < t_min) | (t_max < root))
				return false;
		}

		t_hit = root;
		return true;
	}


	void Evaluate(const Ray& ray, HitRecord& hit)
		const noexcept
	{
		hit.point = ray.At(hit.t);
		const Vector3 outward_normal = (hit.point - center) / radius;
		GetSphereUV(outward_normal, hit.u, hit.v);
		hit.is_front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
		hit.normal = hit.is_front_face ? outward_normal : -outward_normal;
	}


	bool BoundingBox(const float /*t_start*/, const float /*t_end*/, AABB& box)
		const noexcept
	{
		box = AABB(center - Point3(radius, radius, radius),
			center + Point3(radius, radius, radius));
		return true;
	}
};


struct MovingSphere
{
	Point3  center;
	float   radius;
	Vector3 direction;
	float   speed;


	bool Intersect(const Ray& ray, const float t_min, const float t_max, float& t_hit)
		const noexcept
	{
		// Check if there exists any 't' that defines a point P which satisfies
		// the equation (Px - Cx)^2 + (Py - Cy)^2 + (Pz - Cz)^2 = r^2,
		// rewritten as ((O + t * d) - C).((O + t * d) - C) - r^2 = 0,
		// where O is the ray origin and d is the ray direction,
		// which defines a point on the sphere's surface (an intersection)
		// The equation is unrolled as t^2 * d.d + + 2 * t * d.(O - C) + (O - C).(O - C) - r^2 = 0
		// Solving for the unknown t, this means that:
		//  a = |d|^2
		//  b = 2 * d.(O - C)
		//  c = |O - C|^2 - r^2
		// Using h = d.(O - C) such as 2h = b it is possible to simplify the calculation of
		// the discriminant and the solutions of the equation by removing some multiplications.
		const Vector3 oc = ray.origin - GetCenterAt(ray.time);
		const float a = ray.direction.SqrLength();
		const float h = Vector3::Dot(oc, ray.direction);
		const float c = oc.SqrLength() - radius * radius;
		const float discriminant = h * h - a * c;

		if (discriminant < 0) return false;
		const float sqrtd = std::sqrt(discriminant);

		// Find the nearest root that lies in the specified range.
		float root = (-h - sqrtd) / a;
		if ((root < t_min) | (root > t_max))
		{
			root = (-h + sqrtd) / a;
			if ((root < t_min) | (t_max < root))
				return false;
		}

		t_hit = root;
		return true;
	}


	void Evaluate(const Ray& ray, HitRecord& hit)
		const noexcept
	{
		hit.point = ray.At(hit.t);
		const Vector3 outward_normal = (hit.point - GetCenterAt(ray.time)) / radius;
		GetSphereUV(outward_normal, hit.u, hit.v);
		hit.is_front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
		hit.normal = hit.is_front_face ? outward_normal : -outward_normal;
	}


	bool BoundingBox(const float t_start, const float t_end, AABB& box)
		const noexcept
	{
		const Point3 center_start = GetCenterAt(t_start);
		const Point3 center_end = GetCenterAt(t_end);
		const Vector3 offset = Vector3(radius, radius, radius);

		const AABB box_start = AABB((center_start - offset), (center_start + offset));
		const AABB box_end = AABB((center_end - offset), (center_end + offset));
		box = AABB::Combine(box_start, box_end);
		return true;
	}

private:

	Point3 GetCenterAt(float t) const noexcept
	{
		return center + direction * speed * t;
	}
};


struct Rectangle
{
	enum class Type
	{
		Invalid,
		XY,
		XZ,
		YZ
	};

	Type  type = Type::Invalid;
	float k = 0.0;
	float a0 = 0.0, b0 = 0.0;
	float a1 = 0.0, b1 = 0.0;


	bool Intersect(const Ray& ray, const float t_min, const float t_max, float& t_hit)
		const noexcept
	{
		switch (type)
		{
			case Type::XY:
			{
				// We solve the ray equation P(t) = A + t*b to find out the
				// value of t where Pz == k, we can then use this value to calculate
				// the x and y coordinates at the intersection point.
				const float t = (k - ray.origin.z()) / ray.direction.z();
				if (t < t_min || t > t_max)
					return false;

				const float x = ray.origin.x() + t * ray.direction.x();
				const float y = ray.origin.y() + t * ray.direction.y();
				if (x < a0 || x > a1 || y < b0 || y > b1)
					return false;

				t_hit = t;
				return true;
			}

			case Type::XZ:
			{
				// We solve the ray equation P(t) = A + t*b to find out the
				// value of t where Py == k, we can then use this value to calculate
				// the x and z coordinates at the intersection point.
				const float t = (k - ray.origin.y()) / ray.direction.y();
				if (t < t_min || t > t_max)
					return false;

				const float x = ray.origin.x() + t * ray.direction.x();
				const float z = ray.origin.z() + t * ray.direction.z();
				if (x < a0 || x > a1 || z < b0 || z > b1)
					return false;

				t_hit = t;
				return true;
			}

			case Type::YZ:
			{
				// We solve the ray equation P(t) = A + t*b to find out the
				// value of t where Px == k, we can then use this value to calculate
				// the y and z coordinates at the intersection point.
				const float t = (k - ray.origin.x()) / ray.direction.x();
				if (t < t_min || t > t_max)
					return false;

				const float y = ray.origin.y() + t * ray.direction.y();
				const float z = ray.origin.z() + t * ray.direction.z();
				if (y < a0 || y > a1 || z < b0 || z > b1)
					return false;

				t_hit = t;
				return true;
			}

			default: return false;
		}
	}


	void Evaluate(const Ray& ray, HitRecord& hit)
		const noexcept
	{
		switch (type)
		{
			case Type::XY:
			{
				hit.point = ray.At(hit.t);
				hit.u = (hit.point.x() - a0) / (a1 - a0);
				hit.v = (hit.point.y() - b0) / (b1 - b0);
				hit.is_front_face = ray.direction.z() < 0.0;
				hit.normal = hit.is_front_face ? Vector3{ 0, 0, 1 } : Vector3{ 0, 0, -1 };
			} break;

			case Type::XZ:
			{
				hit.point = ray.At(hit.t);
				hit.u = (hit.point.x() - a0) / (a1 - a0);
				hit.v = (hit.point.z() - b0) / (b1 - b0);
				hit.is_front_face = ray.direction.y() < 0.0;
				hit.normal = hit.is_front_face ? Vector3{ 0, 1, 0 } : Vector3{ 0, -1, 0 };
			} break;

			case Type::YZ:
			{
				hit.point = ray.At(hit.t);
				hit.u = (hit.point.y() - a0) / (a1 - a0);
				hit.v = (hit.point.z() - b0) / (b1 - b0);
				hit.is_front_face = ray.direction.x() < 0.0;
				hit.normal = hit.is_front_face ? Vector3{ 1, 0, 0 } : Vector3{ -1, 0, 0 };
			} break;
		}
	}


	bool BoundingBox(const float /*t_start*/, const float /*t_end*/, AABB& box)
		const noexcept
	{
		// The bounding box must have non-zero width in each dimension,
		//  so pad the constant dimension by a small amount.
		switch (type)
		{
			case Type::XY: box = AABB(Point3(a0, b0, k - 0.0001), Point3(a1, b1, k + 0.0001)); return true;
			case Type::XZ: box = AABB(Point3(a0, k - 0.0001, b0), Point3(a1, k + 0.0001, b1)); return true;
			case Type::YZ: box = AABB(Point3(k - 0.0001, a0, b0), Point3(k + 0.0001, a1, b1)); return true;
			default: return false;
		}
	}
};


struct Box
{
	Point3 min;
	Point3 max;


	bool Intersect(const Ray& ray, const float t_min, const float t_max, float& t_hit)
		const noexcept
	{
		t_hit = t_max;

		// Check the 6 faces individually
		// 
		// For each, we solve the ray equation P(t) = A + t*b to find out the
		// value of t where Pz == k, we can then use this value to calculate
		// the x and y coordinates at the intersection point and verify whether
		// it falls inside the box face or not.
		//
		// XY (min)
		float t = (min.z() - ray.origin.z()) / ray.direction.z();
		if (t >= t_min && t < t_hit)
		{
			const float x = ray.origin.x() + t * ray.direction.x();
			const float y = ray.origin.y() + t * ray.direction.y();
			if (x >= min.x() && x <= max.x() && y >= min.y() && y <= max.y())
				t_hit = t;
		}
		// XY (max)
		t = (max.z() - ray.origin.z()) / ray.direction.z();
		if (t >= t_min && t < t_hit)
		{
			const float x = ray.origin.x() + t * ray.direction.x();
			const float y = ray.origin.y() + t * ray.direction.y();
			if (x >= min.x() && x <= max.x() && y >= min.y() && y <= max.y())
				t_hit = t;
		}
		// XZ (min)
		t = (min.y() - ray.origin.y()) / ray.direction.y();
		if (t >= t_min && t < t_hit)
		{
			const float x = ray.origin.x() + t * ray.direction.x();
			const float z = ray.origin.z() + t * ray.direction.z();
			if (x >= min.x() && x <= max.x() && z >= min.z() && z <= max.z())
				t_hit = t;
		}
		// XZ (max)
		t = (max.y() - ray.origin.y()) / ray.direction.y();
		if (t >= t_min && t < t_hit)
		{
			const float x = ray.origin.x() + t * ray.direction.x();
			const float z = ray.origin.z() + t * ray.direction.z();
			if (x >= min.x() && x <= max.x() && z >= min.z() && z <= max.z())
				t_hit = t;
		}
		// YZ (min)
		t = (min.x() - ray.origin.x()) / ray.direction.x();
		if (t >= t_min && t < t_hit)
		{
			const float y = ray.origin.y() + t * ray.direction.y();
			const float z = ray.origin.z() + t * ray.direction.z();
			if (y >= min.y() && y <= max.y() && z >= min.z() && z <= max.z())
				t_hit = t;
		}
		// YZ (max)
		t = (max.x() - ray.origin.x()) / ray.direction.x();
		if (t >= t_min && t < t_hit)
		{
			const float y = ray.origin.y() + t * ray.direction.y();
			const float z = ray.origin.z() + t * ray.direction.z();
			if (y >= min.y() && y <= max.y() && z >= min.z() && z <= max.z())
				t_hit = t;
		}
		return (t_hit >= t_min && t_hit < t_max);
	}


	void Evaluate(const Ray& ray, HitRecord& hit)
		const noexcept
	{
		// Check the value of 't' to find out which face was hit
		const float t_values[6]
		{
			(min.z() - ray.origin.z()) / ray.direction.z(), // XY (min)
			(max.z() - ray.origin.z()) / ray.direction.z(), // XY (max)
			(min.y() - ray.origin.y()) / ray.direction.y(), // XZ (min)
			(max.y() - ray.origin.y()) / ray.direction.y(), // XZ (max)
			(min.x() - ray.origin.x()) / ray.direction.x(), // YZ (min)
			(max.x() - ray.origin.x()) / ray.direction.x()  // YZ (max)
		};
		int face_hit = -1;
		for (int i = 0; i < 6; i++)
		{
			if (hit.t == t_values[i])
			{
				face_hit = i;
				break;
			}
		}

		switch (face_hit)
		{
		case 0:
		{
			hit.point = ray.At(hit.t);
			hit.u = 1.0 - (hit.point.x() - min.x()) / (max.x() - min.x());
			hit.v = (hit.point.y() - min.y()) / (max.y() - min.y());
			hit.normal = { 0, 0, -1 };
			hit.is_front_face = ray.direction.z() > 0.0;
		} break;
		case 1:
		{
			hit.point = ray.At(hit.t);
			hit.u = (hit.point.x() - min.x()) / (max.x() - min.x());
			hit.v = (hit.point.y() - min.y()) / (max.y() - min.y());
			hit.normal = { 0, 0, 1 };
			hit.is_front_face = ray.direction.z() < 0.0;
		} break;
		case 2:
		{
			hit.point = ray.At(hit.t);
			hit.u = 1.0 - (hit.point.x() - min.x()) / (max.x() - min.x());
			hit.v = 1.0 - (hit.point.z() - min.z()) / (max.z() - min.z());
			hit.normal = { 0, -1, 0 };
			hit.is_front_face = ray.direction.y() > 0.0;
		} break;
		case 3:
		{
			hit.point = ray.At(hit.t);
			hit.u = 1.0 - (hit.point.x() - min.x()) / (max.x() - min.x());
			hit.v = (hit.point.z() - min.z()) / (max.z() - min.z());
			hit.normal = { 0, 1, 0 };
			hit.is_front_face = ray.direction.y() < 0.0;
		} break;
		case 4:
		{
			hit.point = ray.At(hit.t);
			hit.u = (hit.point.z() - min.z()) / (max.z() - min.z());
			hit.v = (hit.point.y() - min.y()) / (max.y() - min.y());
			hit.normal = { -1, 0, 0 };
			hit.is_front_face = ray.direction.x() > 0.0;
		} break;
		case 5:
		{
			hit.point = ray.At(hit.t);
			hit.u = 1.0 - (hit.point.z() - min.z()) / (max.z() - min.z());
			hit.v = (hit.point.y() - min.y()) / (max.y() - min.y());
			hit.normal = { 1, 0, 0 };
			hit.is_front_face = ray.direction.x() < 0.0;
		} break;
		}
	}


	bool BoundingBox(const float /*t_start*/, const float /*t_end*/, AABB& box)
		const noexcept
	{
		box = AABB(min, max);
		return true;
	}
};


struct Hittable;	//forward declaration

struct Volume
{
	float neg_inv_density = 0.0;
	std::shared_ptr<const Hittable> boundary;

	// NOTE: implementations at the end of the file because they require
	//  the type Hittable to be fully defined

	bool Intersect(const Ray& ray, const float t_min, const float t_max, float& t_hit)
		const noexcept;
	
	void Evaluate(const Ray& ray, HitRecord& hit)
		const noexcept;

	bool BoundingBox(const float t_start, const float t_end, AABB& box)
		const noexcept;
};


//*************************************************************
// Defines a single data structure that can hold any hittable.
struct Hittable
{
	// NOTE: values of this enum must match with the
	//  index of the corresponding type in HittableData.
	enum class Type
	{
		Sphere = 0,
		MovingSphere = 1,
		Rectangle = 2,
		Box = 3,
		Volume = 4
	};

	struct Rotation
	{
		float sin_theta = 0.0f;
		float cos_theta = 1.0f;

		Rotation() = default;
		Rotation(float angle)
			: sin_theta(std::sin(Deg2Rad(angle)))
			, cos_theta(std::cos(Deg2Rad(angle)))
		{}
	};

	using HittableData = std::variant
	<
		Sphere,				// 16 bytes
		MovingSphere,		// 32 bytes
		Rectangle,			// 24 bytes
		Box,				// 24 bytes
		Volume				// 12 bytes
	>;						//----------
	HittableData data;		// 40 bytes (32 bytes + std::variant<> overhead/alignment)
	Vector3 translation;	// 12 bytes
	Rotation rotation_y;	//  8 bytes
	MaterialID material;	//  4 bytes
							//----------
							// 64 bytes

	bool Intersect(const Ray& ray_in, const float t_min, const float t_max, float& t_hit)
		const noexcept
	{
		Ray ray = ray_in;
		if (HasTranslation()) ray = TranslateRay(ray, translation);
		if (HasRotation()) ray = RotateRay(ray, rotation_y);

		switch (Hittable::Type(data.index()))
		{
			case Type::Sphere:			return std::get<Sphere>(data).Intersect(ray, t_min, t_max, t_hit);
			case Type::MovingSphere:	return std::get<MovingSphere>(data).Intersect(ray, t_min, t_max, t_hit);
			case Type::Rectangle:		return std::get<Rectangle>(data).Intersect(ray, t_min, t_max, t_hit);
			case Type::Box:				return std::get<Box>(data).Intersect(ray, t_min, t_max, t_hit);
			case Type::Volume:			return std::get<Volume>(data).Intersect(ray, t_min, t_max, t_hit);
			default:					return false;
		}
	}

	void Evaluate(const Ray& ray_in, HitRecord& hit)
		const noexcept
	{
		Ray ray = ray_in;
		if (HasTranslation()) ray = TranslateRay(ray, translation);
		if (HasRotation()) ray = RotateRay(ray, rotation_y);

		switch (Hittable::Type(data.index()))
		{
			case Type::Sphere:			std::get<Sphere>(data).Evaluate(ray, hit); break;
			case Type::MovingSphere:	std::get<MovingSphere>(data).Evaluate(ray, hit); break;
			case Type::Rectangle:		std::get<Rectangle>(data).Evaluate(ray, hit); break;
			case Type::Box:				std::get<Box>(data).Evaluate(ray, hit); break;
			case Type::Volume:			std::get<Volume>(data).Evaluate(ray, hit); break;
			default:					break;
		}
		hit.material_id = material;

		if (HasRotation())
		{
			Point3 point = hit.point;
			Vector3 normal = hit.normal;

			// Rotate back the hit point and surface normal
			point[0]  =  rotation_y.cos_theta * hit.point[0]  + rotation_y.sin_theta * hit.point[2];
			point[2]  = -rotation_y.sin_theta * hit.point[0]  + rotation_y.cos_theta * hit.point[2];
			normal[0] =  rotation_y.cos_theta * hit.normal[0] + rotation_y.sin_theta * hit.normal[2];
			normal[2] = -rotation_y.sin_theta * hit.normal[0] + rotation_y.cos_theta * hit.normal[2];

			hit.point = point;
			hit.is_front_face = Vector3::Dot(ray.direction, normal) < 0.0;
			hit.normal = hit.is_front_face ? normal : -normal;
		}
		if (HasTranslation())
		{
			// Translate back the hit point
			hit.point += translation;
		}
	}

	bool BoundingBox(const float t_start, const float t_end, AABB& box)
		const noexcept
	{
		bool success = false;
		switch (Hittable::Type(data.index()))
		{
			case Type::Sphere:			success = std::get<Sphere>(data).BoundingBox(t_start, t_end, box); break;
			case Type::MovingSphere:	success = std::get<MovingSphere>(data).BoundingBox(t_start, t_end, box); break;
			case Type::Rectangle:		success = std::get<Rectangle>(data).BoundingBox(t_start, t_end, box); break;
			case Type::Box:				success = std::get<Box>(data).BoundingBox(t_start, t_end, box); break;
			case Type::Volume:			success = std::get<Volume>(data).BoundingBox(t_start, t_end, box); break;
			default:					break;
		}
		if (!success)
			return false;

		if (HasRotation())
		{
			Point3 min(Infinity, box.min.y(), Infinity);
			Point3 max(-Infinity, box.max.y(), -Infinity);

			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					const float x = i * box.max.x() + (1 - i) * box.min.x();
					const float z = j * box.max.z() + (1 - j) * box.min.z();

					const float newx =  rotation_y.cos_theta * x + rotation_y.sin_theta * z;
					const float newz = -rotation_y.sin_theta * x + rotation_y.cos_theta * z;

					min[0] = std::min(min[0], newx);
					min[2] = std::min(min[2], newz);
					max[0] = std::max(max[0], newx);
					max[2] = std::max(max[2], newz);
				}
			}

			box = AABB(min, max);
		}
		if (HasTranslation())
		{
			box.min += translation;
			box.max += translation;
		}
		return true;
	}


	static void CreateSphere(Hittable& hittable, const Point3& center, const float radius) noexcept
	{
		hittable.data = Sphere{ .center = center, .radius = radius };
	}

	static void CreateMovingSphere(Hittable& hittable, const Point3& center, const float radius,
		const Vector3& direction, const float speed) noexcept
	{
		hittable.data = MovingSphere{ .center = center, .radius = radius, .direction = direction, .speed = speed };
	}

	static void CreateRectangle(Hittable& hittable,	const Point3& p0, const Point3& p1) noexcept
	{
		Rectangle rectangle;
		if (p0.x() == p1.x())
		{
			rectangle.type = Rectangle::Type::YZ;
			rectangle.k  = p0.x();
			rectangle.a0 = p0.y();
			rectangle.b0 = p0.z();
			rectangle.a1 = p1.y();
			rectangle.b1 = p1.z();
		}
		else if (p0.y() == p1.y())
		{
			rectangle.type = Rectangle::Type::XZ;
			rectangle.k = p0.y();
			rectangle.a0 = p0.x();
			rectangle.b0 = p0.z();
			rectangle.a1 = p1.x();
			rectangle.b1 = p1.z();
		}
		else if (p0.z() == p1.z())
		{
			rectangle.type = Rectangle::Type::XY;
			rectangle.k = p0.z();
			rectangle.a0 = p0.x();
			rectangle.b0 = p0.y();
			rectangle.a1 = p1.x();
			rectangle.b1 = p1.y();
		}
		else
		{
			std::cerr << "Only axis-aligned rectangles are supported.\n";
			assert(false);
		}

		hittable.data = rectangle;
	}

	static void CreateBox(Hittable& hittable, const Point3& min, const Point3& max) noexcept
	{
		hittable.data = Box{ .min = min, .max = max };
	}

	static void CreateVolume(Hittable& hittable, const std::shared_ptr<const Hittable> boundary, const float density) noexcept
	{
		hittable.data = Volume{ .neg_inv_density = (-1.0f / density), .boundary = boundary };
	}


	static Type GetTypeFromString(const std::string& str)
	{
		static std::map<std::string, Type> str_type_map
		{
			{ "Sphere"      , Type::Sphere       },
			{ "MovingSphere", Type::MovingSphere },
			{ "Rectangle"   , Type::Rectangle    },
			{ "Box"         , Type::Box          },
			{ "Volume"      , Type::Volume       },
		};
		return str_type_map.at(str);
	}


private:

	bool HasTranslation() const noexcept
	{ 
		return translation.x() != 0.0f || translation.y() != 0.0f || translation.z() != 0.0f;
	}

	bool HasRotation() const noexcept
	{
		return rotation_y.sin_theta != 0.0f && rotation_y.cos_theta != 1.0f;
	}

	Ray TranslateRay(const Ray& ray, const Vector3& offset) const noexcept
	{
		return Ray(ray.origin - offset, ray.direction, ray.time);
	}

	Ray RotateRay(const Ray& ray, const Rotation& rotation) const noexcept
	{
		Point3  origin = ray.origin;
		Vector3 direction = ray.direction;

		// Rotate the ray origin and direction around the Y axis
		origin[0] = rotation.cos_theta * ray.origin[0] - rotation.sin_theta * ray.origin[2];
		origin[2] = rotation.sin_theta * ray.origin[0] + rotation.cos_theta * ray.origin[2];
		direction[0] = rotation.cos_theta * ray.direction[0] - rotation.sin_theta * ray.direction[2];
		direction[2] = rotation.sin_theta * ray.direction[0] + rotation.cos_theta * ray.direction[2];

		return Ray(origin, direction, ray.time);
	}
};
//*************************************************************


bool Volume::Intersect(const Ray& ray, const float t_min, const float t_max, float& t_hit)
	const noexcept
{
	// A ray passing through a volume of constant density can either scatter inside
	// the volume or make it all the way through. The denser the volume, the more
	// likely it is for the ray to be scattered; the probability of the ray scattering
	// over a small distance is P = C * dL, where C is proportional to the optical
	// density of the volume.
	// The code below (given a random number) calculates the distance at which the 
	// scattering of a ray would occur: if that distance is inside the volume boundary
	// it's a hit, otherwise means that there is no "hit".

	float t_hit_enter, t_hit_exit;

	if (!boundary->Intersect(ray, -Infinity, Infinity, t_hit_enter))
		return false;

	if (!boundary->Intersect(ray, t_hit_enter + 0.0001, Infinity, t_hit_exit))
		return false;

	t_hit_enter = std::max(t_hit_enter, t_min);
	t_hit_exit = std::min(t_hit_exit, t_max);

	if (t_hit_enter >= t_hit_exit)
		return false;

	if (t_hit_enter < 0.0)
		t_hit_enter = 0.0;

	const float ray_length = ray.direction.Length();
	const float distance_inside_boundary = (t_hit_exit - t_hit_enter) * ray_length;
	const float hit_distance = neg_inv_density * std::log(Random::GetFloat(0.0, 1.0));

	if (hit_distance > distance_inside_boundary)
		return false;

	t_hit = t_hit_enter + hit_distance / ray_length;
	return true;
}


void Volume::Evaluate(const Ray& ray, HitRecord& hit)
	const noexcept
{
	hit.point = ray.At(hit.t);
	hit.normal = Vector3(1, 0, 0);			// arbitrary
	hit.is_front_face = true;				// also arbitrary
}


bool Volume::BoundingBox(const float t_start, const float t_end, AABB& box)
	const noexcept
{
	return boundary->BoundingBox(t_start, t_end, box);
}
