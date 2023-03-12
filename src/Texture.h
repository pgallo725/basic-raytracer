#pragma once

#include "Common.h"


class Texture
{
public:

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