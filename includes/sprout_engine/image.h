#pragma once

#include "stb_image_write.h"

#include "color.h"

#include <string>
#include <vector>

class Image
{
protected:
    int m_width;
    int m_height;

    std::vector<Color> pixels;

public:
    Image() : m_width(0), m_height(0) {}

    Image(int width, int height) : m_width(width), m_height(height)
    {
        pixels.resize(width * height);
    }

    Color operator()(int x, int y) const
    {
        return pixels.at(y * m_width + x);
    }

    Color& operator()(int x, int y)
    {
        return pixels.at(y * m_width + x);
    }

    Color linearToGamma(const Color& p_pixel)
    {
        Color res{0.f, 0.f, 0.f, 1.f};
        if(p_pixel.r > 0)
            res.r = std::sqrtf(p_pixel.r);
        if(p_pixel.g > 0)
            res.g = std::sqrtf(p_pixel.g);
        if(p_pixel.b > 0)
            res.b = std::sqrtf(p_pixel.b);

        return res;
    }

    void write(const char* filename)
    {
        //colors must be converted from float to unsigned char to match the format
        std::vector<unsigned char> pixels_rgb;
        for (const Color& c : pixels)
        {
            Color toWrite = linearToGamma(c);
            pixels_rgb.push_back(static_cast<unsigned char>(toWrite.r * 255.f));
            pixels_rgb.push_back(static_cast<unsigned char>(toWrite.g * 255.f));
            pixels_rgb.push_back(static_cast<unsigned char>(toWrite.b * 255.f));
            pixels_rgb.push_back(static_cast<unsigned char>(c.a * 255.f));
        }

        stbi_write_png(filename, m_width, m_height, 4, pixels_rgb.data(), m_width * 4);
    }

    inline const Color* data() const { return pixels.data(); }
    inline int width() const { return m_width; }
    inline int height() const { return m_height; }
};