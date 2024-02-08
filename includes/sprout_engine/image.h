#pragma once

#include "stb_image_write.h"

#include <sprout_engine/color.h>

#include <string>
#include <vector>

class Image
{
protected:
	int i_width;
	int i_height;

	std::vector<Color> pixels;

public:
	Image() : i_width(0), i_height(0) {}

	Image(int width, int height) : i_width(width), i_height(height)
	{
		pixels.resize(width * height);
	}

	Color operator()(int row, int col) const
	{
		return pixels.at(row * i_width + col);
	}

	Color& operator()(int row, int col)
	{
		return pixels.at(row * i_width + col);
	}

	void write(const char* filename)
	{
		//colors must be converted from float to unsigned char to match the format
		std::vector<unsigned char> pixels_rgb;
		for (const Color& c : pixels)
		{
			pixels_rgb.push_back((unsigned char)(c.r * 255));
			pixels_rgb.push_back((unsigned char)(c.g * 255));
			pixels_rgb.push_back((unsigned char)(c.b * 255));
			pixels_rgb.push_back((unsigned char)(c.a * 255));
		}

		stbi_write_png(filename, i_width, i_height, 4, pixels_rgb.data(), i_width * 4);
	}

	inline const Color* data() const { return pixels.data(); } 
	inline int width() const { return i_width; }
	inline int height() const { return i_height; }
};