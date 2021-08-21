#pragma once

#include "Hittable.h"
#include "Vector3.h"

class Sphere : public Hittable
{
public:

    Point3 center;
    double radius;

public:

    Sphere() : center(), radius(0.0) {}
    Sphere(Point3 center, double radius) : center(center), radius(radius) {}

    virtual bool Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const override;
};


bool Sphere::Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const
{
    // Check if there exists any 't' that defines a point P which satisfies
    // the equation (Px - Cx)^2 + (Py - Cy)^2 + (Pz - Cz)^2 = r^2,
    // rewritten as ((A + t * b) - C).((A + t * b) - C) - r^2 = 0,
    // which defines a point on the sphere's surface (an intersection)
    Vector3 oc = ray.origin - center;
    double a = ray.direction.SqrLength();
    double b = 2.0 * Vector3::Dot(oc, ray.direction);
    double c = oc.SqrLength() - radius * radius;
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return false;
    double sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the specified range.
    double root = (-b - sqrtd) / (2.0 * a);
    if (root < t_min || root > t_max)
    {
        root = (-b + sqrtd) / (2.0 * a);
        if (root < t_min || t_max < root)
            return false;
    }
    
    hit.t = root;
    hit.point = ray.At(hit.t);
    Vector3 outward_normal = Vector3::Normalized(hit.point - center);
    hit.SetFaceNormal(ray, outward_normal);

    return true;
}
