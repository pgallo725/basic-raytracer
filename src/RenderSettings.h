#pragma once

#include <iostream>
#include <exception>

#include "Common.h"


class RenderSettings
{
private:

    std::string     scene_path = "scene.json";
    std::string     output_path = "render.ppm";
    uint32_t        image_width = 1280;
    uint32_t        image_height = 720;
    uint32_t        samples_per_pixel = 500;
    uint32_t        max_bounces = 50;
    uint32_t        thread_count = 4;
    double          aspect_ratio = 16.0 / 9.0;

public:

    std::string   ScenePath()        const noexcept { return scene_path; }
    std::string   OutputPath()       const noexcept { return output_path; }
    uint32_t      ImageWidth()       const noexcept { return image_width; }
    uint32_t      ImageHeight()      const noexcept { return image_height; }
    uint32_t      SamplesPerPixel()  const noexcept { return samples_per_pixel; }
    uint32_t      MaxBounces()       const noexcept { return max_bounces; }
    uint32_t      ThreadCount()      const noexcept { return thread_count; }
    double        AspectRatio()      const noexcept { return aspect_ratio; }


    static RenderSettings& Get() noexcept
    {
        static RenderSettings settings;     // Static singleton storage for render settings
        return settings;
    }


    void ParseCommandLine(const int argc, const char** const argv)
    {
        if (argc < 5)
            throw std::exception("insufficient number of parameters");

        scene_path = ReadStringParam(argv, 1, "scene");
        output_path = ReadStringParam(argv, 2, "output");
        image_width = ReadUInt32Param(argv, 3, "width");
        image_height = ReadUInt32Param(argv, 4, "height");

        aspect_ratio = double(image_width) / double(image_height);

        int index = 5;
        while (index < argc)
        {
            std::string option = ReadOptionSpecifier(argv, index);
            index += 1;

            if (index == argc)
                throw std::exception(("option '" + option + "' is not followed by a value").c_str());

            if (option.compare("-s") == 0 || option.compare("--samples") == 0)
            {
                samples_per_pixel = ReadUInt32Param(argv, index, "samples");
                index += 1;
            }
            else if (option.compare("-b") == 0 || option.compare("--bounces") == 0)
            {
                max_bounces = ReadUInt32Param(argv, index, "bounces");
                index += 1;
            }
            else if (option.compare("-t") == 0 || option.compare("--threads") == 0)
            {
                thread_count = ReadUInt32Param(argv, index, "threads");
                index += 1;
            }
            else
            {
                std::cerr << "WARNING: "
                    << '\'' << option << "' is not a supported option specifier and will be skipped"
                    << std::endl;
            }
        }
    }

    void Print() const noexcept
    {
        std::cout << '\n'
            << "RENDER SETTINGS:\n\n"
            << " Scene File: \t\t"          << scene_path                               << '\n'
            << " Output File: \t\t"         << output_path                              << '\n'
            << " Image Resolution: \t"      << image_width << 'x' << image_height       << '\n'
            << " Samples per Pixel: \t"     << samples_per_pixel                        << '\n'
            << " Max. Bounces: \t\t"        << max_bounces                              << '\n'
            << " Num. Threads: \t\t"        << thread_count                             << '\n'
            << std::endl;
    }


private:

    // Prevent the class from being instantiated by making the constructor private
    RenderSettings() {};


    inline static std::string ReadStringParam(const char** const argv, const int index, const std::string& name)
    {
        return std::string(argv[index]);
    }

    inline static uint32_t ReadUInt32Param(const char** const argv, const int index, const std::string& name)
    {
        std::string value_str = std::string(argv[index]);
        try
        {
            int value_int = std::stoi(value_str);
            if (value_int <= 0)
                throw std::exception();
            
            return uint32_t(value_int);
        }
        catch (std::exception e)
        {
            std::string error = "\'" + value_str + "' is not a valid value for '" + name + '\'';
            throw std::exception(error.c_str());
        }
    }

    inline static std::string ReadOptionSpecifier(const char** const argv, const int index)
    {
        std::string option = std::string(argv[index]);

        if (!(option.starts_with("--") || option.starts_with('-')))
            throw std::exception("option specifiers must begin with '-' or '--' (e.g. --samples)");

        return option;
    }
};
