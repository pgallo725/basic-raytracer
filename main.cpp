#include <iostream>

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
            double r = double(i) / (image_width - 1);
            double g = double(j) / (image_height - 1);
            double b = 0.25;

            int ir = static_cast<int>(r * 255.999);
            int ig = static_cast<int>(g * 255.999);
            int ib = static_cast<int>(b * 255.999);

            std::cout << ir << ' ' << ig << ' ' << ib << ' ';
        }
        std::cout << '\n';
    }
    std::cerr << "\nDone!\n";
}