#pragma once

#include <thread>
#include <semaphore>

#include "Common.h"
#include "Scene.h"


class RenderThread
{
public:

    // Output array, represents the latest scanline rendered by this thread.
    Color* pixels;

private:

    std::thread m_thread;
    std::binary_semaphore m_renderSemaphore;
    std::binary_semaphore m_resultSemaphore;
	const Scene& ref_scene;
	const int m_width, m_height;
	const int m_samples;
    const int m_maxBounces;

public:

	RenderThread(const uint32_t thread_id, const Scene& scene,
        const int width, const int height, const int samples, const int bounces)
		: m_renderSemaphore(1), m_resultSemaphore(0), ref_scene(scene),
        m_width(width), m_height(height), m_samples(samples), m_maxBounces(bounces)
	{
        pixels = new Color[m_width];

        // Instantiate a thread to run the render loop with a unique seed
        Random::SeedCurrentThread(thread_id);
        m_thread = std::thread(&RenderThread::RenderLoop, this);
	}

    ~RenderThread()
    {
        delete[] pixels;
    }

    inline void BeginRender()
    {
        m_renderSemaphore.release();
    }

    inline void WaitRender()
    {
        m_resultSemaphore.acquire();
    }

    inline void Join()
    {
        if (m_thread.joinable())
            m_thread.join();
    }

private:

	void RenderLoop()
	{
        for (int j = m_height - 1; j >= 0; j--)
        {
            // Wait for render command by the main thread.
            m_renderSemaphore.acquire();

            for (int i = 0; i < m_width; i++)
            {
                // Gather multiple samples per pixel, and accumulate them.
                Color pixel = Color(0, 0, 0);
                for (int s = 0; s < m_samples; s++)
                {
                    double u = (i + Random::GetDouble(0.0, 1.0)) / (m_width - 1);
                    double v = (j + Random::GetDouble(0.0, 1.0)) / (m_height - 1);

                    pixel += RayColor(ref_scene.camera.GetRay(u, v), ref_scene, m_maxBounces);
                }

                // Average the collected samples to get the color for the output pixel.
                pixel /= m_samples;
                pixels[i] = pixel;
            }

            // Notify the main thread about the available rendering result.
            m_resultSemaphore.release();
        }
	}


    Color RayColor(const Ray& ray, const Hittable& world, int bounces_left) const
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
};
