#include <iostream>

#include "Vector3.h"
#include "Color.h"

int main()
{
    // Image size

    const int image_width = 256;
    const int image_height = 256;

    // Render PPM image

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--)
    {
        std::cerr << "\rRendering scanline " << (image_height - j) << '/' << image_height;
        for (int i = 0; i < image_width; i++)
        {
            Color pixel = Color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
            WriteColor(std::cout, pixel);
        }
        std::cout << '\n';
    }
    std::cerr << "\nDone!\n";
}