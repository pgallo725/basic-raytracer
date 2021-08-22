#include "Common.h"

#include <iostream>

#include "Color.h"
#include "Camera.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Material.h"


Color RayColor(const Ray& ray, const Hittable& world, int bounces_left)
{
    HitRecord hit;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (bounces_left <= 0)
        return Color(0.0, 0.0, 0.0);

    // Intersect the ray against the world geometry.
    if (world.Hit(ray, 0.001, Infinity, hit))
    {
        Ray scattered;
        Color attenuation;

        // Scatter the ray against the surface (based on material properties)
        if (hit.material->Scatter(ray, hit, attenuation, scattered))
            return attenuation * RayColor(scattered, world, bounces_left - 1);
        else return Color(0.0, 0.0, 0.0);
    }

    // A blue-to-white gradient depending on ray Y coordinate as background.
    Vector3 unit_dir = Vector3::Normalized(ray.direction);
    double t = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}


int main()
{
    // IMAGE PROPERTIES

    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_bounces = 50;

    // WORLD

    HittableList world;

    auto material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<Dielectric>(1.5);
    auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

    world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), -0.45, material_left));
    world.Add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));

    // CAMERA

    Camera camera(Point3(-2, 2, 1), Point3(0, 0, -1), Vector3(0, 1, 0), 90.0, aspect_ratio);

    // RENDER PPM IMAGE

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // Traverse the screen from the upper left hand corner, along left-to-right scan lines,
    // and cast a ray from the "eye" to each pixel in the viewport, to compute its color.
    for (int j = image_height - 1; j >= 0; j--)
    {
        std::cerr << "\rRendering scanline " << (image_height - j) << '/' << image_height;
        for (int i = 0; i < image_width; i++)
        {
            // Gather multiple samples per pixel, and accumulate them.
            Color pixel = Color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++)
            {
                double u = (i + RandomDouble(0.0, 1.0)) / (image_width - 1);
                double v = (j + RandomDouble(0.0, 1.0)) / (image_height - 1);

                pixel += RayColor(camera.GetRay(u, v), world, max_bounces);
            }
            // Average the collected samples when writing the final colored pixel.
            WriteColor(std::cout, pixel, samples_per_pixel);
        }
        std::cout << '\n';
    }
    std::cerr << "\nDone!\n";
}