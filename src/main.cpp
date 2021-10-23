#include <iostream>
#include <exception>
#include <chrono>

#include "Common.h"
#include "Image.h"
#include "RenderSettings.h"
#include "RenderThread.h"
#include "Scene.h"


std::string usageString(const char* program)
{
    std::string str = "Usage: ";
    str.append(program);                                                                        // Program name
    str.append(" <scene> <output> <width> <height>");                                           // Required parameters
    str.append(" [-s / --samples <value>][-b / --bounces <value>][-t / --threads <value>]");    // Optional parameters
    return str;
}


int main(int argc, const char** argv)
{
    // PARSE ARGUMENTS

    RenderSettings& settings = RenderSettings::Get();
    try
    {
        settings.ParseCommandLine(argc, argv);
        settings.Print();
    }
    catch (std::exception e)
    {
        std::cerr << "ERROR: " << e.what() << '\n' << usageString(argv[0]) << std::endl;
        return -1;
    }

    // LOAD SCENE

    Scene scene = Scene::Load(settings.ScenePath());

    // RENDER IMAGE

    auto start_time = std::chrono::steady_clock::now();

    Image image(settings.OutputPath(), settings.ImageWidth(), settings.ImageHeight());

    std::vector<std::unique_ptr<RenderThread>> render_threads;
    for (uint32_t id = 0; id < settings.ThreadCount(); id++)
    {
        // Calculate the number of samples that each render thread will compute.
        int sample_count = int(settings.SamplesPerPixel() / settings.ThreadCount());
        if (id < settings.SamplesPerPixel() % settings.ThreadCount()) sample_count++;

        render_threads.emplace_back(std::make_unique<RenderThread>(id,
            scene, settings.ImageWidth(), settings.ImageHeight(), sample_count, settings.MaxBounces()));
    }

    // Run a batch of render threads in parallel, each one rendering a subset of samples
    // of the entire image, and then combine their results together.
    for (int j = settings.ImageHeight() - 1; j >= 0; j--)
    {
        std::cout << "\rRendering scanline " << (settings.ImageHeight() - j) << '/' << settings.ImageHeight();

        // Start the rendering loop for the current scanline on all threads.
        for (auto& thread : render_threads)
            thread->BeginRender();

        // Wait for all threads to complete their rendering job.
        for (auto& thread : render_threads)
            thread->WaitRender();

        for (int i = 0; i < settings.ImageWidth(); i++)
        {
            // Accumulate the samples collected by all render threads.
            Color pixel(0, 0, 0);
            for (int t = 0; t < settings.ThreadCount(); t++)
                pixel += render_threads[t]->pixels[i];

            // Average samples to get the value of the final output pixel.
            pixel /= settings.ThreadCount();

            image.Write(pixel);
        }
    }

    // Join all render threads to avoid zombies.
    for (auto& thread : render_threads)
        thread->Join();

    image.Close();

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "\nDone! (" << (duration / 1000.0) << "s)\n";
}
