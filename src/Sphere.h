#pragma once

#include "Common.h"
#include "Hittable.h"
#include "Material.h"
#include "Vector3.h"


class Sphere : public Hittable
{
public:

    Point3 center;
    double radius;
    std::shared_ptr<Material> material;

public:

    Sphere() : center(), radius(0.0) {}
    Sphere(Point3 center, double radius, std::shared_ptr<Material> material)
        : center(center), radius(radius), material(material) {}

    virtual bool Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const override;
};


bool Sphere::Hit(const Ray& ray, double t_min, double t_max, HitRecord& hit) const
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
    Vector3 oc = ray.origin - center;
    double a = ray.direction.SqrLength();
    double h = Vector3::Dot(oc, ray.direction);
    double c = oc.SqrLength() - radius * radius;
    double discriminant = h * h - a * c;

    if (discriminant < 0) return false;
    double sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the specified range.
    double root = (-h - sqrtd) / a;
    if ((root < t_min) | (root > t_max))
    {
        root = (-h + sqrtd) / a;
        if ((root < t_min) | (t_max < root))
            return false;
    }
    
    hit.t = root;
    hit.point = ray.At(hit.t);
    Vector3 outward_normal = (hit.point - center) / radius;
    hit.is_front_face = Vector3::Dot(ray.direction, outward_normal) < 0.0;
    hit.normal = hit.is_front_face ? outward_normal : -outward_normal;
    hit.material = material.get();

    return true;
}


// JSON deserialization function
void from_json(const json& j, Sphere& s)
{
    j.at("center").get_to<Point3>(s.center);
    j.at("radius").get_to<double>(s.radius);
    s.material = Material::Deserialize(j.at("material"));
}
