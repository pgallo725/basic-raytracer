#pragma once

#include "Common.h"
#include "RenderSettings.h"
#include "RenderThread.h"


class Renderer
{
private:

    const uint32_t m_width;
    const uint32_t m_height;
	std::vector<std::unique_ptr<RenderThread>> m_renderThreads;

public:

	Renderer(const Scene& scene, const RenderSettings& settings)
        : m_width(settings.ImageWidth()), m_height(settings.ImageHeight())
	{
		for (uint32_t id = 0; id < settings.ThreadCount(); id++)
		{
			// Calculate the number of samples that each render thread will compute.
			int samples = int(settings.SamplesPerPixel() / settings.ThreadCount());
			if (id < settings.SamplesPerPixel() % settings.ThreadCount()) samples++;

			m_renderThreads.emplace_back(std::make_unique<RenderThread>(id,
				scene, settings.ImageWidth(), settings.ImageHeight(), samples, settings.MaxBounces()));
		}
	}

	void Render(Image& image) const noexcept
	{
        // Run a batch of render threads in parallel, each one rendering a subset of samples
        // of the entire image, and then combine their results together.
        for (uint32_t j = 0; j < m_height; j++)
        {
            std::cout << "\rRendering scanline " << (j + 1) << '/' << m_height;

            // Start the rendering loop for the current scanline on all threads.
            for (const auto& thread : m_renderThreads)
                thread->BeginRender();

            // Wait for all threads to complete their rendering job.
            for (const auto& thread : m_renderThreads)
                thread->WaitRender();

            for (uint32_t i = 0; i < m_width; i++)
            {
                Color pixel(0, 0, 0);

                // Accumulate the samples collected by all render threads.
                for (const auto& thread : m_renderThreads)
                    pixel += thread->pixels[i];

                // Average samples to get the value of the final output pixel.
                pixel /= m_renderThreads.size();

                image.Write(pixel);
            }
        }

        // Join all render threads to avoid zombies.
        for (const auto& thread : m_renderThreads)
            thread->Join();
	}
};
