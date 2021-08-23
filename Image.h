#pragma once

#include "Common.h"

#include <fstream>
#include <string>


class Image
{
public:

	static std::ofstream Create(const std::string& name, int width, int height, int max_value)
	{
		// Create the image file
		std::ofstream image(name, std::ios::out | std::ios::binary);

		// Write the header information to the file
		image << "P6\n" << width << ' ' << height << '\n' << max_value << '\n';

		return image;
	}

	static void WritePixel(std::ofstream& image, const Color& pixel)
	{
		// Gamma-correct the color values for gamma=2.0.
		double r = std::sqrt(pixel.x());
		double g = std::sqrt(pixel.y());
		double b = std::sqrt(pixel.z());

		// Write the translated [0,255] value of each color component to the image.
		image << static_cast<unsigned char>(256 * Clamp(r, 0.0, 0.999999))
			<< static_cast<unsigned char>(256 * Clamp(g, 0.0, 0.999999))
			<< static_cast<unsigned char>(256 * Clamp(b, 0.0, 0.999999));
	}

	static void Close(std::ofstream& image)
	{
		image.close();
	}
};
