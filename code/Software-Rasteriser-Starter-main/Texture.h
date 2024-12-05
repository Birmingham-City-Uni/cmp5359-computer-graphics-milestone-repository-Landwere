#pragma once
#include "geometry.h"
#include <string>
#include <iostream>
#include "stb_image.h"
inline double clamp(double x, double min, double max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
/* source code from Peter Shirley (https://raytracing.github.io/books/RayTracingTheNextWeek.html) starts here: */
//texture class for other classes to derive from


//image loader class with help of stb to get the information from the image 
class Texture {
public:
    const static int bytes_per_pixel = 3;

    Texture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    Texture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }


    Vec3f value(Vec2f st) {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return Vec3f(0.25, 0.25, 0.25);

        // Clamp input texture coordinates to [0,1] x [1,0]
        st.x = clamp(st.x, 0.0, 1.0);
        st.y = 1.0 - clamp(st.y, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(st.x * width);
        auto j = static_cast<int>(st.y * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return Vec3f(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    unsigned char* data;
    int width, height;
    int bytes_per_scanline;
};