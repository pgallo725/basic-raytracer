#pragma once

#include "Common.h"
#include "Vector3.h"

#include <iostream>

void WriteColor(std::ostream& out, Color pixel, int samples)
{
    double r = pixel.x();
    double g = pixel.y();
    double b = pixel.z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples;
    r = std::sqrt(scale * r);
    g = std::sqrt(scale * g);
    b = std::sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * Clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * Clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * Clamp(b, 0.0, 0.999)) << ' ';
}
