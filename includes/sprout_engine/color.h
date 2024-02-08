#pragma once

#include <glm/glm.hpp>

class Color
{
protected:

public:
	float r;
	float g;
	float b;
	float a;

	Color() : r(1.f), g(1.f), b(1.f), a(1.f) {};
	Color(float r, float g, float b) : r(r), g(g), b(b), a(1.f) {};
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {};

	inline static Color Red() { return Color(1.f, 0.f, 0.f); };
	inline static Color Green() { return Color(0.f, 1.f, 0.f); };
	inline static Color Blue() { return Color(0.f, 0.f, 1.f); };
	inline static Color Black() { return Color(0.f, 0.f, 0.f); };
	inline static Color White() { return Color(1.f, 1.f, 1.f); };
};