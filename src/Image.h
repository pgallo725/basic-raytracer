#pragma once

#include <fstream>

#include "Common.h"


class Image
{
private:

	uint64_t  m_width;
	uint64_t  m_height;
	uint8_t * m_pixels;

public:

	Image(const uint32_t width, const uint32_t height)
		: m_width(width), m_height(height)
	{
		m_pixels = new uint8_t[m_width * m_height * 3];
	}

	~Image()
	{
		delete[] m_pixels;
	}


	uint32_t GetWidth() const noexcept 
	{
		return static_cast<uint32_t>(m_width);
	}

	uint32_t GetHeight() const noexcept
	{
		return static_cast<uint32_t>(m_height);
	}


	void SetPixel(const uint32_t x, const uint32_t y, const Color& pixel) noexcept
	{
		// Gamma-correct the color values for gamma=2.0.
		const float r = std::sqrt(pixel.x());
		const float g = std::sqrt(pixel.y());
		const float b = std::sqrt(pixel.z());

		// Compute the index of the pixel in the array.
		const uint64_t i = (uint64_t(y) * m_width + uint64_t(x)) * 3;

		// Write the translated [0,255] value of each color component to the image.
		m_pixels[i  ] = static_cast<uint8_t>(256 * Clamp(r, 0.0, 0.999999));
		m_pixels[i+1] = static_cast<uint8_t>(256 * Clamp(g, 0.0, 0.999999));
		m_pixels[i+2] = static_cast<uint8_t>(256 * Clamp(b, 0.0, 0.999999));
	}


	void WriteToDisk(const std::string& filename) const
	{
		// Create the image file
		std::ofstream file(filename, std::ios::out | std::ios::binary);

		if (!file.is_open() || file.bad())
			throw std::exception("cannot create or open output image file for writing");

		// Write the PPM header information to the image file
		file << "P6\n" << m_width << ' ' << m_height << '\n' << 255 << '\n';

		// Write pixels to the image file
		for (uint64_t y = 0, i = 0; y < m_height; y++)
			for (uint64_t x = 0; x < m_width; x++, i+=3)
				file << m_pixels[i] << m_pixels[i+1] << m_pixels[i+2];

		file.close();
	}
};
