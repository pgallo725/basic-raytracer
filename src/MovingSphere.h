#pragma once

#include "Common.h"
#include "Hittable.h"
#include "Material.h"


class MovingSphere : public Hittable
{
public:

    Point3 center;
    double radius;
    Vector3 direction;
    double speed;
    std::shared_ptr<Material> material;

public:

    MovingSphere() : center(), radius(0.0), direction(), speed(0.0) {}
    MovingSphere(Point3 center, double radius, Vector3 direction, double speed, std::shared_ptr<Material> material)
        : center(center), radius(radius), direction(direction), speed(speed), material(material) 
    {}


    // Ray-MovingSphere intersection checking.
    virtual bool Hit(const Ray& ray, const double t_min, const double t_max, HitRecord& hit)
        const noexcept override final
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
        const double a = ray.direction.SqrLength();
        const double h = Vector3::Dot(oc, ray.direction);
        const double c = oc.SqrLength() - radius * radius;
        const double discriminant = h * h - a * c;

        if (discriminant < 0) return false;
        const double sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the specified range.
        double root = (-h - sqrtd) / a;
        if ((root < t_min) | (root > t_max))
        {
            root = (-h + sqrtd) / a;
            if ((root < t_min) | (t_max < root))
                return false;
        }

        // Fill the HitRecord structure with all the info about the intersection.
        hit.t = root;
        hit.point = ray.At(hit.t);
        const Vector3 outward_normal = (hit.point - GetCenterAt(ray.time)) / radius;
        GetSphereUV(outward_normal, hit.u, hit.v);
        hit.is_front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
        hit.normal = hit.is_front_face ? outward_normal : -outward_normal;
        hit.material = material.get();

        return true;
    }


    // MovingSphere bounding box.
    virtual bool BoundingBox(const double t_start, const double t_end, AABB& box)
        const noexcept override final
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

    Point3 GetCenterAt(double t) const noexcept
    {
        return center + direction * speed * t;
    }
};
