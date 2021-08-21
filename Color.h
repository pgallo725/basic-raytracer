#pragma once

#include "Vector3.h"

#include <iostream>

void WriteColor(std::ostream& out, Color pixel)
{
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel.x()) << ' '
        << static_cast<int>(255.999 * pixel.y()) << ' '
        << static_cast<int>(255.999 * pixel.z()) << ' ';
}
