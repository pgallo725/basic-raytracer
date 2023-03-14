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

	virtual Color Sample(const double u, const double v, const Point3& p) const noexcept = 0;
};


class SolidTexture : public Texture
{
public:

	SolidTexture() = default;
	SolidTexture(const Color& color) 
		: color(color) {}
	SolidTexture(const double r, const double g, const double b) 
		: color({ r, g, b }) {}

	virtual Color Sample(const double /*u*/, const double /*v*/, const Point3& /*p*/)
		const noexcept { return color; }

private:

	Color color;
};


class CheckerTexture : public Texture
{
public:

	CheckerTexture() = default;
	CheckerTexture(const Color& even, const Color& odd, const double scale)
		: even(even), odd(odd), scale(scale) {}

	virtual Color Sample(const double /*u*/, const double /*v*/, const Point3& p)
		const noexcept
	{
		const double sines = std::sin(scale * p.x()) * std::sin(scale * p.y()) * std::sin(scale * p.z());
		return (sines > 0 ? even : odd);
	}

private:

	Color even;
	Color odd;
	double scale = 1.0;
};


class NoiseTexture : public Texture
{
public:

	NoiseTexture() = default;
	NoiseTexture(const Color& color, double scale)
		: color(color), scale(scale) {}

	virtual Color Sample(const double /*u*/, const double /*v*/, const Point3& p)
		const noexcept
	{
		// The Perlin noise function returns values in [-1, 1], rescale to [0, 1]
		return color * (perlin.Noise(scale * p) + 1.0) * 0.5;
	}

private:

	Perlin perlin;
	Color  color;
	double scale = 1.0;
};


class MarbleTexture : public Texture
{
public:

	MarbleTexture() = default;
	MarbleTexture(const Color& color, double scale, double turbulence)
		: color(color), scale(scale), turbulence(turbulence) {}

	virtual Color Sample(const double /*u*/, const double /*v*/, const Point3& p)
		const noexcept
	{
		// Make the color proportional to a sine function, but use turbulence to adjust
		// the phase (so it shifts x in sin(x)), which makes the strips ondulate.
		return color * (1.0 + std::sin(scale * p.z() + turbulence * perlin.TurbulentNoise(p))) * 0.5;
	}

private:

	Perlin perlin;
	Color  color;
	double scale = 1.0;
	double turbulence = 1.0;
};


class ImageTexture : public Texture
{
public:

	ImageTexture() = default;
	ImageTexture(const std::string& filename)
	{
		data = std::unique_ptr<stbi_uc>(stbi_load(filename.c_str(), &width, &height, &components, 3));

		if (!data)
		{
			std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
			width = height = components = 0;
		}
	}

	virtual Color Sample(const double u, const double v, const Point3& /*p*/)
		const noexcept
	{
		// If we have no texture data, then return solid pink as a debugging aid.
		if (data == nullptr)
			return Color(1, 0, 1);

		// Clamp input texture coordinates to [0,1] x [1,0]
		double uu = Clamp(u, 0.0, 1.0);
		double vv = 1.0 - Clamp(v, 0.0, 1.0);  // Flip V to image coordinates

		int i = static_cast<int>(uu * width);
		int j = static_cast<int>(vv * height);

		// Clamp integer mapping, since actual coordinates should be less than 1.0
		if (i >= width)  i = width - 1;
		if (j >= height) j = height - 1;

		const int bytes_pixel = components;
		const int bytes_scanline = width * bytes_pixel;

		const stbi_uc* pixel = data.get() + j * bytes_scanline + i * bytes_pixel;

		const double scale = 1.0 / 255.0;
		return Color(scale * pixel[0], scale * pixel[1], scale * pixel[2]);
	}

private:

	std::unique_ptr<stbi_uc> data;
	int width = 0, height = 0;
	int components = 0;
};