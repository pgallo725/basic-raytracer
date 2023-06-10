#pragma once

#include "Common.h"

#ifdef _MSC_VER
    #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#ifdef _MSC_VER
    #pragma warning (pop)
#endif


class Texture
{
public:

	virtual ~Texture() = default;

	virtual Color Sample(const float u, const float v, const Point3& p) const noexcept = 0;
};


class SolidTexture : public Texture
{
public:

	Color color;

public:

	SolidTexture() = default;
	SolidTexture(const Color& color) 
		: color(color) {}
	SolidTexture(const float r, const float g, const float b) 
		: color({ r, g, b }) {}

	virtual Color Sample(const float /*u*/, const float /*v*/, const Point3& /*p*/)
		const noexcept { return color; }
};


class CheckerTexture : public Texture
{
public:

	Color even;
	Color odd;
	float scale = 1.0;

public:

	CheckerTexture() = default;
	CheckerTexture(const Color& even, const Color& odd, const float scale)
		: even(even), odd(odd), scale(scale) {}

	virtual Color Sample(const float /*u*/, const float /*v*/, const Point3& p)
		const noexcept
	{
		const float sines = std::sin(scale * p.x()) * std::sin(scale * p.y()) * std::sin(scale * p.z());
		return (sines > 0 ? even : odd);
	}
};


class NoiseTexture : public Texture
{
public:

	Perlin perlin;
	Color  color;
	float  scale = 1.0;

public:

	NoiseTexture() = default;
	NoiseTexture(const Color& color, float scale)
		: color(color), scale(scale) {}

	virtual Color Sample(const float /*u*/, const float /*v*/, const Point3& p)
		const noexcept
	{
		// The Perlin noise function returns values in [-1, 1], rescale to [0, 1]
		return color * (perlin.Noise(scale * p) + 1.0) * 0.5;
	}
};


class MarbleTexture : public Texture
{
public:

	Perlin perlin;
	Color  color;
	float  scale = 1.0;
	float  turbulence = 1.0;

public:

	MarbleTexture() = default;
	MarbleTexture(const Color& color, float scale, float turbulence)
		: color(color), scale(scale), turbulence(turbulence) {}

	virtual Color Sample(const float /*u*/, const float /*v*/, const Point3& p)
		const noexcept
	{
		// Make the color proportional to a sine function, but use turbulence to adjust
		// the phase (so it shifts x in sin(x)), which makes the strips ondulate.
		return color * (1.0 + std::sin(scale * p.z() + turbulence * perlin.TurbulentNoise(p))) * 0.5;
	}
};


class ImageTexture : public Texture
{
public:

	std::string filename;
	std::unique_ptr<stbi_uc> data;
	int width = 0, height = 0;
	int components = 0;

public:

	ImageTexture() = default;
	ImageTexture(const std::string& filename)
		: filename(filename)
	{
		data = std::unique_ptr<stbi_uc>(stbi_load(filename.c_str(), &width, &height, &components, 3));

		if (!data)
		{
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height = components = 0;
		}
	}

	virtual Color Sample(const float u, const float v, const Point3& /*p*/)
		const noexcept
	{
		// If we have no texture data, then return solid pink as a debugging aid.
		if (data == nullptr)
			return Color(1, 0, 1);

		// Clamp input texture coordinates to [0,1] x [1,0]
		float uu = Clamp(u, 0.0, 1.0);
		float vv = 1.0 - Clamp(v, 0.0, 1.0);  // Flip V to image coordinates

		size_t i = static_cast<size_t>(uu * width);
		size_t j = static_cast<size_t>(vv * height);

		// Clamp integer mapping, since actual coordinates should be less than 1.0
		if (i >= width)  i = width - 1;
		if (j >= height) j = height - 1;

		const size_t bytes_pixel = components;
		const size_t bytes_scanline = width * bytes_pixel;

		const stbi_uc* pixel = data.get() + j * bytes_scanline + i * bytes_pixel;

		const float scale = 1.0 / 255.0;
		return Color(scale * pixel[0], scale * pixel[1], scale * pixel[2]);
	}
};