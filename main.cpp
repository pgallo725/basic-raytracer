#include <iostream>
#include <chrono>

#include "Common.h"
#include "Parameters.h"
#include "Image.h"
#include "Camera.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Material.h"
#include "RenderThread.h"


HittableList RandomScene() 
{
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) 
    {
        for (int b = -11; b < 11; b++) 
        {
            auto choose_mat = Random::GetDouble(0, 1);
            Point3 center(a + 0.9 * Random::GetDouble(0, 1), 0.2, b + 0.9 * Random::GetDouble(0, 1));

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) 
            {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) 
                {
                    // diffuse
                    auto albedo = Random::GetColor(0, 1) * Random::GetColor(0, 1);
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) 
                {
                    // metal
                    auto albedo = Random::GetColor(0.5, 1);
                    auto fuzz = Random::GetDouble(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else 
                {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}



int main(int argc, const char** argv)
{
    // PARSE ARGUMENTS

    Parameters params;
    if (!params.ParseCommandLine(argc, argv))
        return -1;

    params.Print();

    // WORLD

    HittableList world = RandomScene();

    // CAMERA

    Point3 look_from(13, 2, 3);
    Point3 look_at(0, 0, 0);
    Vector3 view_up(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    Camera camera(look_from, look_at, view_up, 20, params.aspect_ratio, aperture, dist_to_focus);

    // RENDER IMAGE

    auto start_time = std::chrono::steady_clock::now();

    std::ofstream image = Image::Create("render.ppm", params.image_width, params.image_height, 255);

    std::vector<std::unique_ptr<RenderThread>> render_threads;
    for (uint32_t id = 0; id < params.thread_count; id++)
    {
        // Calculate the number of samples that each render thread will compute.
        int sample_count = int(params.samples_per_pixel / params.thread_count);
        if (id < params.samples_per_pixel % params.thread_count) sample_count++;

        render_threads.emplace_back(std::make_unique<RenderThread>(id,
            world, camera, params.image_width, params.image_height, sample_count, params.max_bounces));
    }

    // Run a batch of render threads in parallel, each one rendering a subset of samples
    // of the entire image, and then combine their results together.
    for (int j = params.image_height - 1; j >= 0; j--)
    {
        std::cout << "\rRendering scanline " << (params.image_height - j) << '/' << params.image_height;

        // Start the rendering loop for the current scanline on all threads.
        for (auto& thread : render_threads)
            thread->BeginRender();

        // Wait for all threads to complete their rendering job.
        for (auto& thread : render_threads)
            thread->WaitRender();

        for (int i = 0; i < params.image_width; i++)
        {
            // Accumulate the samples collected by all render threads.
            Color pixel(0, 0, 0);
            for (int t = 0; t < params.thread_count; t++)
                pixel += render_threads[t]->pixels[i];

            // Average samples to get the value of the final output pixel.
            pixel /= params.thread_count;

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
