#pragma once

#include <thread>

#include "Common.h"
#include "Scene.h"
#include "Image.h"
#include "RenderSettings.h"


class RenderThread
{
private:

    std::thread    m_thread;
    const uint32_t m_threadID;
    const Scene&   ref_scene;
    Image&         ref_image;
    const uint32_t m_samples;
    const uint32_t m_bounces;

    std::atomic_uint32_t& ref_counter;

public:

    RenderThread(const uint32_t thread_id,
        const Scene& scene,
        Image& image,
        const uint32_t samples,
        const uint32_t bounces,
        std::atomic_uint32_t& counter) :
        m_threadID(thread_id),
        m_thread(std::thread(&RenderThread::RenderLoop, this)),
        ref_scene(scene),
        ref_image(image),
        m_samples(samples),
        m_bounces(bounces),
        ref_counter(counter)
    {
    }

    void Join()
    {
        if (m_thread.joinable())
            m_thread.join();
    }

private:

    void RenderLoop() noexcept
    {
        // Initialize the random number generator for this thread with a unique seed.
        Random::SeedCurrentThread(m_threadID);

        while (true)
        {
            // Grab the next scanline index from the atomic counter, and increment it.
            const uint32_t j = ref_counter.fetch_add(1);

            // Notify the main thread that a new scanline is being rendered.
            ref_counter.notify_all();

            // Stop the render loop when reached beyond the last scanline.
            if (j >= ref_image.GetHeight())
                break;

            // Render each pixel in the scanline.
            for (uint32_t i = 0; i < ref_image.GetWidth(); i++)
            {
                Color pixel = Color(0, 0, 0);

                // Gather multiple samples per pixel, and accumulate them.
                for (uint32_t s = 0; s < m_samples; s++)
                {
                    const double u = (i + Random::GetDouble(0.0, 1.0)) / ((double)ref_image.GetWidth() - 1);
                    const double v = 1.0 - (j + Random::GetDouble(0.0, 1.0)) / ((double)ref_image.GetHeight() - 1);  // flip image vertically

                    pixel += RayColor(ref_scene.camera.GetRay(u, v), ref_scene, m_bounces);
                }

                // Average the collected samples to get the color for the output pixel.
                pixel /= m_samples;
                ref_image.SetPixel(i, j, pixel);
            }
        }
    }


    inline Color RayColor(const Ray& ray, const Scene& scene, const uint32_t bounces) const noexcept
    {
        // If we've reached the ray bounce limit, no more light is gathered.
        if (bounces == 0)
            return Color(0.0, 0.0, 0.0);

        HitRecord hit;

        // Intersect the ray against the world geometry,
        //  if it hits nothing return the background color.
        if (!scene.Hit(ray, 0.001, Infinity, hit))
            return scene.background;

        Ray   scattered;
        Color attenuation;
        Color emitted = hit.material->Emitted(ray, hit);

        // Scatter the ray against the surface (based on material properties).
        if (!hit.material->Scatter(ray, hit, attenuation, scattered))
            return emitted;

        // Terminate recursion if the energy of the ray drops to almost zero.
        if (attenuation.NearZero())
            return emitted;

        return emitted + attenuation * RayColor(scattered, scene, bounces - 1);
    }
};


class Renderer
{
public:

	static void Render(const Scene& scene, Image& image, const RenderSettings& settings) noexcept
	{
        std::atomic_uint32_t counter{ 0 };
        std::vector<std::unique_ptr<RenderThread>> threads;
        threads.reserve(settings.ThreadCount());

        // Spawn a given number of worker threads, which will render individual scanlines
        // of the final image. Each thread grabs the index of the next scanline to process
        // from the atomic counter, avoiding any expensive synchronization.
        for (uint32_t id = 0; id < settings.ThreadCount(); id++)
        {
            threads.emplace_back(std::make_unique<RenderThread>(id,
                scene, image, settings.SamplesPerPixel(), settings.MaxBounces(), counter));
        }

        // Update the scanline counter in the command line UI.
        uint32_t value = 0;
        while (value < image.GetHeight())
        {
            // Wait on the atomic counter to be updated by worker threads.
            counter.wait(value);
            value = counter.load();

            const uint32_t scanline = std::min(value + 1, image.GetHeight());
            std::cout << "\rRendering scanline " << scanline << '/' << image.GetHeight();
        }

        // Join all render threads to avoid zombies.
        for (const auto& thread : threads)
            thread->Join();
	}
};
