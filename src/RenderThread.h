#pragma once

#include <thread>
#include <semaphore>

#include "Common.h"
#include "Scene.h"


class RenderThread
{
public:

    // Output array, represents the latest scanline rendered by this thread.
    Color* const pixels;

private:

    std::thread m_thread;
    std::binary_semaphore m_renderSemaphore;
    std::binary_semaphore m_resultSemaphore;
	const Scene& ref_scene;
	const uint32_t m_width, m_height;
	const uint32_t m_samples;
    const uint32_t m_maxBounces;

public:

	RenderThread(const uint32_t thread_id, 
        const Scene& scene,
        const uint32_t width, 
        const uint32_t height,
        const uint32_t samples, 
        const uint32_t bounces) :
		m_renderSemaphore(0),
        m_resultSemaphore(0),
        ref_scene(scene),
        m_width(width), m_height(height),
        m_samples(samples),
        m_maxBounces(bounces),
        pixels(new Color[width])
	{
        // Instantiate a thread to run the render loop with a unique seed.
        Random::SeedCurrentThread(thread_id);
        m_thread = std::thread(&RenderThread::RenderLoop, this);
	}

    ~RenderThread()
    {
        delete[] pixels;
    }

    void BeginRender() noexcept
    {
        m_renderSemaphore.release();
    }

    void WaitRender() noexcept
    {
        m_resultSemaphore.acquire();
    }

    void Join()
    {
        if (m_thread.joinable())
            m_thread.join();
    }

private:

	void RenderLoop() noexcept
	{
        for (int32_t j = m_height - 1; j >= 0; j--)
        {
            // Wait for render command by the main thread.
            m_renderSemaphore.acquire();

            for (uint32_t i = 0; i < m_width; i++)
            {
                Color pixel = Color(0, 0, 0);

                // Gather multiple samples per pixel, and accumulate them.
                for (uint32_t s = 0; s < m_samples; s++)
                {
                    double u = (i + Random::GetDouble(0.0, 1.0)) / ((double)m_width - 1);
                    double v = (j + Random::GetDouble(0.0, 1.0)) / ((double)m_height - 1);

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


    inline Color RayColor(const Ray& ray, const Scene& scene, const uint32_t bounces) const noexcept
    {
        // If we've reached the ray bounce limit, no more light is gathered.
        if (bounces == 0)
            return Color(0.0, 0.0, 0.0);

        HitRecord hit;

        // Intersect the ray against the world geometry.
        if (scene.Hit(ray, 0.001, Infinity, hit))
        {
            Ray scattered;
            Color attenuation;

            // Scatter the ray against the surface (based on material properties).
            if (hit.material->Scatter(ray, hit, attenuation, scattered))
                return attenuation * RayColor(scattered, scene, bounces - 1);
            else return Color(0.0, 0.0, 0.0);
        }
        else
        {
            // A blue-to-white gradient depending on ray Y coordinate as background.
            const double t = 0.5 * ((ray.direction.y() / ray.direction.Length()) + 1.0);
            return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
        }
    }
};
