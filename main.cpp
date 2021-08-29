#include <iostream>

#include "Common.h"
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
            auto choose_mat = RandomDouble(0, 1);
            Point3 center(a + 0.9 * RandomDouble(0, 1), 0.2, b + 0.9 * RandomDouble(0, 1));

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) 
            {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) 
                {
                    // diffuse
                    auto albedo = Color::Random(0, 1) * Color::Random(0, 1);
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) 
                {
                    // metal
                    auto albedo = Color::Random(0.5, 1);
                    auto fuzz = RandomDouble(0, 0.5);
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


int main()
{
    // IMAGE PROPERTIES

    const double aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_bounces = 50;
    const int thread_count = 6;

    // WORLD

    HittableList world = RandomScene();

    // CAMERA

    Point3 look_from(13, 2, 3);
    Point3 look_at(0, 0, 0);
    Vector3 view_up(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    Camera camera(look_from, look_at, view_up, 20, aspect_ratio, aperture, dist_to_focus);

    // RENDER IMAGE

    std::ofstream image = Image::Create("render.ppm", image_width, image_height, 255);

    std::vector<std::unique_ptr<RenderThread>> render_threads;
    for (int i = 0; i < thread_count; i++)
    {
        // Calculate the number of samples that each render thread will compute.
        int sample_count = int(samples_per_pixel / thread_count);
        if (i < samples_per_pixel % thread_count) sample_count++;

        render_threads.emplace_back(std::make_unique<RenderThread>(
            world, camera, image_width, image_height, sample_count, max_bounces));
    }

    // Run a batch of render threads in parallel, each one rendering a subset of samples
    // of the entire image, and then combine their results together.
    for (int j = image_height - 1; j >= 0; j--)
    {
        std::cout << "\rRendering scanline " << (image_height - j) << '/' << image_height;

        // Start the rendering loop for the current scanline on all threads.
        for (auto& thread : render_threads)
            thread->BeginRender();

        // Wait for all threads to complete their rendering job.
        for (auto& thread : render_threads)
            thread->WaitRender();

        for (int i = 0; i < image_width; i++)
        {
            // Accumulate the samples collected by all render threads.
            Color pixel(0, 0, 0);
            for (int t = 0; t < thread_count; t++)
                pixel += render_threads[t]->pixels[i];

            // Average samples to get the value of the final output pixel.
            pixel /= thread_count;

            Image::WritePixel(image, pixel);
        }
    }

    // Join all render threads to avoid zombies.
    for (auto& thread : render_threads)
        thread->Join();

    Image::Close(image);
    std::cout << "\nDone!\n";
}
