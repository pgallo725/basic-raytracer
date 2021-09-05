#pragma once

#include <string>
#include <exception>
#include <iostream>

#include "Common.h"


struct Parameters
{
    uint32_t    image_width         = 1280;
    uint32_t    image_height        = 720;
    uint32_t    samples_per_pixel   = 500;
    uint32_t    max_bounces         = 50;
    uint32_t    thread_count        = 4;
    double      aspect_ratio        = 16.0 / 9.0;


    bool ParseCommandLine(const int argc, const char** const argv)
    {
        if (argc < 3)
        {
            std::cerr << "ERROR: insufficient number of parameters\n"
                << "Usage: " << argv[0] << " <width> <height> [-s/--samples <value>] [-b/--bounces <value>] [-t/--threads <value>]"
                << std::endl;

            return false;
        }

        try
        {
            image_width = ReadUInt32Param(argv, 1, "width");
            image_height = ReadUInt32Param(argv, 2, "height");

            aspect_ratio = double(image_width) / double(image_height);

            int index = 3;
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

            return true;
        }
        catch (std::exception e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl;

            return false;
        }
    }

    void Print() const
    {
        std::cout << '\n'
            << "RENDER OPTIONS:\n\n"
            << " Image Resolution: \t"      << image_width << 'x' << image_height       << '\n'
            << " Samples per Pixel: \t"     << samples_per_pixel                        << '\n'
            << " Max. Bounces: \t\t"        << max_bounces                              << '\n'
            << " Num. Threads: \t\t"        << thread_count                             << '\n'
            << std::endl;
    }


private:

    uint32_t ReadUInt32Param(const char** const argv, const int index, const std::string& name) const
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

    std::string ReadOptionSpecifier(const char** const argv, const int index) const
    {
        std::string option = std::string(argv[index]);

        if (!(option.starts_with("--") || option.starts_with('-')))
            throw std::exception("option specifiers must begin with '-' or '--' (e.g. --samples)");

        return option;
    }

};

