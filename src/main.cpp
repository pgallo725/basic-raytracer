#include <iostream>
#include <exception>
#include <chrono>

#include "Common.h"
#include "Image.h"
#include "JsonDeserializer.h"
#include "RenderSettings.h"
#include "Renderer.h"
#include "Scene.h"


std::string usageString(const char* program)
{
    std::string str = "Usage: ";
    str.append(program);                                                                        // Program name
    str.append(" <scene> <output> <width> <height>");                                           // Required parameters
    str.append(" [-s / --samples <value>] [-b / --bounces <value>] [-t / --threads <value>]");  // Optional parameters
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

    Scene scene = JsonDeserializer::LoadScene(settings.ScenePath());

    // RENDER IMAGE

    auto start_time = std::chrono::steady_clock::now();

    Image image(settings.OutputPath(), settings.ImageWidth(), settings.ImageHeight());

    Renderer renderer(scene, settings);
    renderer.Render(image);

    // FINISH

    image.Close();

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "\nDone! (" << (duration / 1000.0) << "s)\n";
}
