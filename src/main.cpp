#include <iostream>
#include <exception>
#include <chrono>

#include "Common.h"
#include "Image.h"
#include "JsonDeserializer.h"
#include "RenderSettings.h"
#include "Renderer.h"
#include "Scene.h"


int main(int argc, const char** argv)
{
    // PARSE ARGUMENTS

    RenderSettings& settings = RenderSettings::Get();
    try
    {
        settings.ParseCommandLine(argc, argv);
        settings.Print();
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << '\n' 
            << "Usage: " << argv[0] << " <scene> <output> <width> <height> "                    // Required parameters
            << "[-s / --samples <value>] [-b / --bounces <value>] [-t / --threads <value>]"     // Optional parameters
            << std::endl;

        return -1;
    }

    // LOAD SCENE

    Scene scene;
    try
    {
        scene = JsonDeserializer::LoadScene(settings.ScenePath());
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return -1;
    }

    // BUILD BVH STRUCTURE

    scene.BuildBVH(scene.camera.GetTimeShutterOpen(), scene.camera.GetTimeShutterClose());

    // RENDER IMAGE

    const auto start_time = std::chrono::steady_clock::now();

    Image image(settings.ImageWidth(), settings.ImageHeight());

    Renderer::Render(scene, image, settings);

    // FINISH

    try
    {
        image.WriteToDisk(settings.OutputPath());
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return -1;
    }

    const auto end_time = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "\nDone! (" << (duration / 1000.0) << "s)\n";
}
