#pragma once

#include <fstream>

#include "Common.h"


class Image
{
private:

	std::ofstream file;

public:

	Image(const std::string& name, const uint32_t width, const uint32_t height)
		: file(name, std::ios::out | std::ios::binary)	// Create the image file
	{
		// Write the PPM header information to the image file
		file << "P6\n" << width << ' ' << height << '\n' << 255 << '\n';
	}

	void Write(const Color& pixel)
	{
		// Gamma-correct the color values for gamma=2.0.
		double r = std::sqrt(pixel.x());
		double g = std::sqrt(pixel.y());
		double b = std::sqrt(pixel.z());

		// Write the translated [0,255] value of each color component to the image.
		file << static_cast<uint8_t>(256 * Clamp(r, 0.0, 0.999999))
			 << static_cast<uint8_t>(256 * Clamp(g, 0.0, 0.999999))
			 << static_cast<uint8_t>(256 * Clamp(b, 0.0, 0.999999));
	}

	void Close()
	{
		file.close();
	}
};
