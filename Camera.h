#pragma once

#include "Common.h"

class Camera
{
private:

	Point3 origin;
	Point3 lower_left_corner;
	Vector3 horizontal;
	Vector3 vertical;

public:

	Camera(Point3 lookFrom,
        Point3 lookAt, 
        Vector3 view_up,
        double v_fov,        // vertical field-of-view in degrees
        double aspect_ratio)
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

        // Derive a view direction from the 2 points lookFrom and lookAt
        Vector3 w = Vector3::Normalized(lookFrom - lookAt);
        // Then, using the provided world up vector, derive the right direction
        Vector3 u = Vector3::Normalized(Vector3::Cross(view_up, w));
        // Finally, complete the orthonormal basis by computing the actual view up vector
        Vector3 v = Vector3::Cross(w, u);

        // The “eye” is placed at the provided lookFrom point. 
        // The distance between the projection plane and the eye is set to one unit (“focal length”)
        // along the view direction (-w, for convention).

        origin = lookFrom;
        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
	}

    Ray GetRay(double s, double t) const
    {
        return Ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
    }
};