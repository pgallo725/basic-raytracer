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

	Camera(double aspect_ratio)
	{
        // Viewport vertical coordinates span from -1.0 (bottom) to 1.0 (top), 
        // while the horizontal coordinates are still symmetrical -X (left) to X (right)
        // but the exact values are computed based on the aspect ration, to keep standard
        // square pixel spacing.

        double viewport_height = 2.0;
        double viewport_width = aspect_ratio * viewport_height;
        double focal_length = 1.0f;

        // The “eye” is placed at (0,0,0). The y-axis goes up and the x-axis to the right.
        // In order to respect the convention of a right handed coordinate system, 
        // into the screen is the negative z-axis.
        // The distance between the projection plane and the eye is set to one unit (“focal length”).

        origin = Point3(0, 0, 0);
        horizontal = Vector3(viewport_width, 0, 0);
        vertical = Vector3(0, viewport_height, 0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focal_length);
	}

    Ray GetRay(double u, double v) const
    {
        return Ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
};