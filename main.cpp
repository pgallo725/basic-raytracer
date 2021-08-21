#include "Common.h"

#include <iostream>

#include "Color.h"
#include "HittableList.h"
#include "Sphere.h"


Color RayColor(const Ray& ray, const Hittable& world)
{
    // Intersect the ray against the world geometry.
    HitRecord hit;
    if (world.Hit(ray, 0.0, Infinity, hit))
    {
        // Unit normals have components in [-1, 1], map them to [0, 1] color space
        return 0.5 * Color(hit.normal + Vector3(1, 1, 1));
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

    // WORLD

    HittableList world = HittableList();
    world.Add(std::make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.Add(std::make_shared<Sphere>(Point3(0, -100.5, -1), 100));

    // CAMERA
    // The “eye” is placed at (0,0,0). The y-axis goes up and the x-axis to the right.
    // In order to respect the convention of a right handed coordinate system, 
    // into the screen is the negative z-axis.
    // The distance between the projection plane and the eye is set to one unit (“focal length”).

    // Viewport vertical coordinates span from -1.0 (bottom) to 1.0 (top), 
    // while the horizontal coordinates are still symmetrical -X (left) to X (right)
    // but the exact values are computed based on the aspect ration, to keep standard
    // square pixel spacing.

    double viewport_height = 2.0;
    double viewport_width = aspect_ratio * viewport_height;
    double focal_length = 1.0f;

    Point3 origin = Point3(0, 0, 0);
    Vector3 horizontal = Vector3(viewport_width, 0, 0);
    Vector3 vertical = Vector3(0, viewport_height, 0);
    Vector3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focal_length);

    // RENDER PPM IMAGE

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // Traverse the screen from the upper left hand corner, along left-to-right scan lines,
    // and cast a ray from the "eye" to each pixel in the viewport, to compute its color.
    for (int j = image_height - 1; j >= 0; j--)
    {
        std::cerr << "\rRendering scanline " << (image_height - j) << '/' << image_height;
        for (int i = 0; i < image_width; i++)
        {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Ray ray = Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            Color pixel = RayColor(ray, world);
            WriteColor(std::cout, pixel);
        }
        std::cout << '\n';
    }
    std::cerr << "\nDone!\n";
}