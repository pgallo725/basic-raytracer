#include <iostream>

#include "Vector3.h"
#include "Color.h"
#include "Ray.h"


bool HitSphere(const Point3& center, const double radius, const Ray& ray)
{
    // Check if there exists any 't' that defines a point P which satisfies
    // the equation (Px - Cx)^2 + (Py - Cy)^2 + (Pz - Cz)^2 = r^2,
    // rewritten as ((A + t * b) - C).((A + t * b) - C) - r^2 = 0,
    // which defines a point on the sphere's surface (an intersection)
    Vector3 oc = ray.origin - center;
    double a = Vector3::Dot(ray.direction, ray.direction);
    double b = 2.0 * Vector3::Dot(oc, ray.direction);
    double c = Vector3::Dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

Color RayColor(const Ray& r)
{
    // A red sphere.
    if (HitSphere(Point3(0, 0, -1), 0.5, r))
        return Color(1.0, 0.0, 0.0);

    // A blue-to-white gradient depending on ray Y coordinate as background.
    Vector3 unit_dir = Vector3::Normalized(r.direction);
    auto t = 0.5 * (unit_dir.y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}


int main()
{
    // IMAGE PROPERTIES

    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

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
            Color pixel = RayColor(ray);
            WriteColor(std::cout, pixel);
        }
        std::cout << '\n';
    }
    std::cerr << "\nDone!\n";
}