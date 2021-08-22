#include "Common.h"

#include <iostream>

#include "Color.h"
#include "Camera.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Material.h"


HittableList RandomScene() 
{
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) 
    {
        for (int b = -11; b < 11; b++) 
        {
            auto choose_mat = RandomDouble(0, 1);
            Point3 center(a + 0.9 * RandomDouble(0, 1), 0.2, b + 0.9 * RandomDouble(0, 1));

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) 
            {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) 
                {
                    // diffuse
                    auto albedo = Color::Random(0, 1) * Color::Random(0, 1);
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) 
                {
                    // metal
                    auto albedo = Color::Random(0.5, 1);
                    auto fuzz = RandomDouble(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else 
                {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}


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

    const double aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_bounces = 50;

    // WORLD

    HittableList world = RandomScene();

    // CAMERA

    Point3 look_from(13, 2, 3);
    Point3 look_at(0, 0, 0);
    Vector3 view_up(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    Camera camera(look_from, look_at, view_up, 20, aspect_ratio, aperture, dist_to_focus);

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