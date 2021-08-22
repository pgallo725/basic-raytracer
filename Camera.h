#pragma once

#include "Common.h"

class Camera
{
private:

	Point3 origin;
	Point3 lower_left_corner;
	Vector3 horizontal;
	Vector3 vertical;
    Vector3 v, u, w;
    double lens_radius;

public:

	Camera(Point3 look_from,
        Point3 look_at, 
        Vector3 view_up,
        double v_fov,        // vertical field-of-view in degrees
        double aspect_ratio,
        double aperture,
        double focus_distance)
	{
        // The height of the viewport can be calculated from the FOV
        // using simple trigonometry formulas.
        double theta = Deg2Rad(v_fov);
        double h = std::tan(theta / 2);

        // Viewport vertical coordinates span from -tan(theta/2) (bottom) to tan(theta/2) (top), 
        // while the horizontal coordinates are still symmetrical -X (left) to X (right)
        // but the exact values are computed based on the aspect ration, to keep standard
        // square pixel spacing.

        double viewport_height = 2.0 * h;
        double viewport_width = aspect_ratio * viewport_height;

        // Derive a view direction from the 2 points look_from and look_at
        w = Vector3::Normalized(look_from - look_at);
        // Then, using the provided world up vector, derive the right direction
        u = Vector3::Normalized(Vector3::Cross(view_up, w));
        // Finally, complete the orthonormal basis by computing the actual view up vector
        v = Vector3::Cross(w, u);

        // The �eye� is placed at the provided look_from point. 
        // The distance between the projection plane and the eye is set to one unit (�focal length�)
        // along the view direction (-w, for convention).

        origin = look_from;
        horizontal = focus_distance * viewport_width * u;
        vertical = focus_distance * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_distance * w;

        lens_radius = aperture / 2;
	}

    Ray GetRay(double s, double t) const
    {
        // In order to accomplish defocus blur, generate random scene rays
        // originating from inside a disk centered at the lookfrom point. 
        // The larger the radius, the greater the defocus blur.
        Vector3 rd = lens_radius * Vector3::RandomInUnitDisk();
        Vector3 offset = u * rd.x() + v * rd.y();
        return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - (origin + offset));
    }
};