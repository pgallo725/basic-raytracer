#pragma once

#include <exception>
#include <iostream>

#include "Common.h"


class RenderSettings
{
private:

    // Static storage for global render settings
    static RenderSettings settings;

    std::string scene_path = "scene.json";
    uint32_t    image_width = 1280;
    uint32_t    image_height = 720;
    uint32_t    samples_per_pixel = 500;
    uint32_t    max_bounces = 50;
    uint32_t    thread_count = 4;
    double      aspect_ratio = 16.0 / 9.0;

public:

    static bool ParseCommandLine(const int argc, const char** const argv)
    {
        if (argc < 4)
        {
            std::cerr << "ERROR: insufficient number of parameters\n"
                << "Usage: " << argv[0] << " <scene> <width> <height> [-s/--samples <value>] [-b/--bounces <value>] [-t/--threads <value>]"
                << std::endl;

            return false;
        }

        try
        {
            settings.scene_path = ReadStringParam(argv, 1, "scene");
            settings.image_width = ReadUInt32Param(argv, 2, "width");
            settings.image_height = ReadUInt32Param(argv, 3, "height");

            settings.aspect_ratio = double(settings.image_width) / double(settings.image_height);

            int index = 4;
            while (index < argc)
            {
                std::string option = ReadOptionSpecifier(argv, index);
                index += 1;

                if (index == argc)
                    throw std::exception(("option '" + option + "' is not followed by a value").c_str());

                if (option.compare("-s") == 0 || option.compare("--samples") == 0)
                {
                    settings.samples_per_pixel = ReadUInt32Param(argv, index, "samples");
                    index += 1;
                }
                else if (option.compare("-b") == 0 || option.compare("--bounces") == 0)
                {
                    settings.max_bounces = ReadUInt32Param(argv, index, "bounces");
                    index += 1;
                }
                else if (option.compare("-t") == 0 || option.compare("--threads") == 0)
                {
                    settings.thread_count = ReadUInt32Param(argv, index, "threads");
                    index += 1;
                }
                else
                {
                    std::cerr << "WARNING: "
                        << '\'' << option << "' is not a supported option specifier and will be skipped"
                        << std::endl;
                }
            }

            return true;
        }
        catch (std::exception e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl;

            return false;
        }
    }

    static void Print()
    {
        std::cout << '\n'
            << "RENDER SETTINGS:\n\n"
            << " Scene File: \t\t"          << settings.scene_path                                      << '\n'
            << " Image Resolution: \t"      << settings.image_width << 'x' << settings.image_height     << '\n'
            << " Samples per Pixel: \t"     << settings.samples_per_pixel                               << '\n'
            << " Max. Bounces: \t\t"        << settings.max_bounces                                     << '\n'
            << " Num. Threads: \t\t"        << settings.thread_count                                    << '\n'
            << std::endl;
    }

    inline static std::string   ScenePath()         { return settings.scene_path; }
    inline static uint32_t      ImageWidth()        { return settings.image_width; }
    inline static uint32_t      ImageHeight()       { return settings.image_height; }
    inline static uint32_t      SamplesPerPixel()   { return settings.samples_per_pixel; }
    inline static uint32_t      MaxBounces()        { return settings.max_bounces; }
    inline static uint32_t      ThreadCount()       { return settings.thread_count; }
    inline static double        AspectRatio()       { return settings.aspect_ratio; }

private:

    // Prevent the class from being instantiated by making the constructor private
    RenderSettings() {};

    static std::string ReadStringParam(const char** const argv, const int index, const std::string& name)
    {
        return std::string(argv[index]);
    }

    static uint32_t ReadUInt32Param(const char** const argv, const int index, const std::string& name)
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

    static std::string ReadOptionSpecifier(const char** const argv, const int index)
    {
        std::string option = std::string(argv[index]);

        if (!(option.starts_with("--") || option.starts_with('-')))
            throw std::exception("option specifiers must begin with '-' or '--' (e.g. --samples)");

        return option;
    }
};
