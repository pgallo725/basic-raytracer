#pragma once

#include <iostream>
#include <exception>

#include "Common.h"


class RenderSettings
{
private:

    std::string     m_scenePath = "scene.json";
    std::string     m_outputPath = "render.ppm";
    uint32_t        m_imageWidth = 1280;
    uint32_t        m_imageHeight = 720;
    uint32_t        m_samplesPerPixel = 500;
    uint32_t        m_maxBounces = 50;
    uint32_t        m_threadCount = 4;
    double          m_aspectRatio = 16.0 / 9.0;

public:

    std::string   ScenePath()        const noexcept { return m_scenePath; }
    std::string   OutputPath()       const noexcept { return m_outputPath; }
    uint32_t      ImageWidth()       const noexcept { return m_imageWidth; }
    uint32_t      ImageHeight()      const noexcept { return m_imageHeight; }
    uint32_t      SamplesPerPixel()  const noexcept { return m_samplesPerPixel; }
    uint32_t      MaxBounces()       const noexcept { return m_maxBounces; }
    uint32_t      ThreadCount()      const noexcept { return m_threadCount; }
    double        AspectRatio()      const noexcept { return m_aspectRatio; }


    static RenderSettings& Get() noexcept
    {
        static RenderSettings settings;     // Static singleton storage for render settings
        return settings;
    }


    void ParseCommandLine(const int argc, const char** const argv)
    {
        if (argc < 5)
            throw std::exception("insufficient number of parameters");

        m_scenePath = ReadStringParam(argv, 1, "scene");
        m_outputPath = ReadStringParam(argv, 2, "output");
        m_imageWidth = ReadUInt32Param(argv, 3, "width");
        m_imageHeight = ReadUInt32Param(argv, 4, "height");

        m_aspectRatio = double(m_imageWidth) / double(m_imageHeight);

        int index = 5;
        while (index < argc)
        {
            std::string option = ReadOptionSpecifier(argv, index);
            index += 1;

            if (index == argc)
                throw std::exception(("option '" + option + "' is not followed by a value").c_str());

            if (option.compare("-s") == 0 || option.compare("--samples") == 0)
            {
                m_samplesPerPixel = ReadUInt32Param(argv, index, "samples");
                index += 1;
            }
            else if (option.compare("-b") == 0 || option.compare("--bounces") == 0)
            {
                m_maxBounces = ReadUInt32Param(argv, index, "bounces");
                index += 1;
            }
            else if (option.compare("-t") == 0 || option.compare("--threads") == 0)
            {
                m_threadCount = ReadUInt32Param(argv, index, "threads");
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
            << " Scene File: \t\t"          << m_scenePath                              << '\n'
            << " Output File: \t\t"         << m_outputPath                             << '\n'
            << " Image Resolution: \t"      << m_imageWidth << 'x' << m_imageHeight     << '\n'
            << " Samples per Pixel: \t"     << m_samplesPerPixel                        << '\n'
            << " Max. Bounces: \t\t"        << m_maxBounces                             << '\n'
            << " Num. Threads: \t\t"        << m_threadCount                            << '\n'
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
