#include <iostream>
#include <chrono>

#include "Common.h"
#include "Image.h"
#include "RenderSettings.h"
#include "RenderThread.h"
#include "Scene.h"


int main(int argc, const char** argv)
{
    // PARSE ARGUMENTS

    if (!RenderSettings::ParseCommandLine(argc, argv))
        return -1;

    RenderSettings::Print();

    // LOAD SCENE

    Scene scene = Scene::Load(RenderSettings::ScenePath());

    // RENDER IMAGE

    auto start_time = std::chrono::steady_clock::now();

    std::ofstream image = Image::Create("render.ppm", RenderSettings::ImageWidth(), RenderSettings::ImageHeight(), 255);

    std::vector<std::unique_ptr<RenderThread>> render_threads;
    for (uint32_t id = 0; id < RenderSettings::ThreadCount(); id++)
    {
        // Calculate the number of samples that each render thread will compute.
        int sample_count = int(RenderSettings::SamplesPerPixel() / RenderSettings::ThreadCount());
        if (id < RenderSettings::SamplesPerPixel() % RenderSettings::ThreadCount()) sample_count++;

        render_threads.emplace_back(std::make_unique<RenderThread>(id,
            scene, RenderSettings::ImageWidth(), RenderSettings::ImageHeight(), sample_count, RenderSettings::MaxBounces()));
    }

    // Run a batch of render threads in parallel, each one rendering a subset of samples
    // of the entire image, and then combine their results together.
    for (int j = RenderSettings::ImageHeight() - 1; j >= 0; j--)
    {
        std::cout << "\rRendering scanline " << (RenderSettings::ImageHeight() - j) << '/' << RenderSettings::ImageHeight();

        // Start the rendering loop for the current scanline on all threads.
        for (auto& thread : render_threads)
            thread->BeginRender();

        // Wait for all threads to complete their rendering job.
        for (auto& thread : render_threads)
            thread->WaitRender();

        for (int i = 0; i < RenderSettings::ImageWidth(); i++)
        {
            // Accumulate the samples collected by all render threads.
            Color pixel(0, 0, 0);
            for (int t = 0; t < RenderSettings::ThreadCount(); t++)
                pixel += render_threads[t]->pixels[i];

            // Average samples to get the value of the final output pixel.
            pixel /= RenderSettings::ThreadCount();

            Image::WritePixel(image, pixel);
        }
    }

    // Join all render threads to avoid zombies.
    for (auto& thread : render_threads)
        thread->Join();

    Image::Close(image);

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "\nDone! (" << (duration / 1000.0) << "s)\n";
}
