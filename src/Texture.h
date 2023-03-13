#pragma once

#include "Common.h"


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