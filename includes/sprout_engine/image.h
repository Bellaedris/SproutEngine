#pragma once
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <sprout_engine/color.h>

#include <string>

class Image
{
protected:
	int width;
	int height;

	std::vector<Color> pixels;

public:
	Image() : width(0), height(0) {}

	Image(int width, int height) : width(width), height(height)
	{
		pixels.resize(width * height);
	}

	Color operator()(int row, int col) const
	{
		return pixels.at(row * width + col);
	}

	Color& operator()(int row, int col)
	{
		return pixels.at(row * width + col);
	}

	void write(const char* filename)
	{
		stbi_write_png(filename, width, height, 4, pixels.data(), width * 4);
	}

	inline const Color* data() const { return pixels.data(); } 
	inline int width() const { return width; }
	inline int height() const { return height; }
};