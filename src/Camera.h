#pragma once

#include "Common.h"
#include "RenderSettings.h"


class Camera
{
private:

	Point3 m_origin;
	Point3 m_lowerLeftCorner;
	Vector3 m_horizontal;
	Vector3 m_vertical;
    Vector3 m_view, m_viewRight, m_viewUp;
    double m_lensRadius;
    double m_timeStart, m_timeEnd;      // Shutter open/close times

public:

    Camera() : Camera(Point3(0, 0, 0), Point3(0, 0, 1), Point3(0, 1, 0), 20, 0.1, 10, 0.0, 1.0) {};

	Camera(const Point3& look_from,
        const Point3& look_at, 
        const Vector3& world_up,
        const double v_fov,        // Vertical field-of-view (in degrees)
        const double aperture,
        const double focus_distance,
        const double time_start,
        const double time_end)
	{
        // The height of the viewport can be calculated from the FOV with simple trigonometry.
        const double theta = Deg2Rad(v_fov);
        const double h = std::tan(theta / 2.0);

        // Viewport vertical coordinates span from -tan(theta/2) (bottom) to tan(theta/2) (top), 
        // while the horizontal coordinates are still symmetrical -X (left) to X (right) but the
        // exact values are computed based on the aspect ratio, to keep standard square pixels.

        const RenderSettings& settings = RenderSettings::Get();

        const double viewport_height = 2.0 * h;
        const double viewport_width = settings.AspectRatio() * viewport_height;

        // Derive a view direction from the 2 points look_from and look_at
        m_view = Vector3::Normalized(look_from - look_at);
        // Then, using the provided world up vector, derive the right direction
        m_viewRight = Vector3::Normalized(Vector3::Cross(world_up, m_view));
        // Finally, complete the orthonormal basis by computing the actual view up vector
        m_viewUp = Vector3::Cross(m_view, m_viewRight);

        // The “eye” is placed at the provided look_from point. 
        // The distance between the projection plane and the eye is set to one unit (“focal length”)
        // along the view direction (-m_view, for convention).

        m_origin = look_from;
        m_horizontal = focus_distance * viewport_width * m_viewRight;
        m_vertical = focus_distance * viewport_height * m_viewUp;
        m_lowerLeftCorner = m_origin - m_horizontal / 2.0 - m_vertical / 2.0 - focus_distance * m_view;

        m_lensRadius = aperture / 2.0;
        m_timeStart = time_start;
        m_timeEnd = time_end;
	}

    Ray GetRay(const double s, const double t) const noexcept
    {
        // In order to accomplish defocus blur, generate random scene rays
        // originating from inside a disk centered at the look_from point. 
        // The larger the radius, the greater the defocus blur.
        Vector3 rd = m_lensRadius * Random::GetVectorInUnitDisk();
        Vector3 offset = m_viewRight * rd.x() + m_viewUp * rd.y();
        return Ray(
            m_origin + offset,
            m_lowerLeftCorner + s * m_horizontal + t * m_vertical - (m_origin + offset),
            Random::GetDouble(m_timeStart, m_timeEnd)
        );
    }
};
