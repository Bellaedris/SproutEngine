#pragma once

#include <glm/glm.hpp>

using Color = glm::vec4;

inline Color lerp(const Color& a, const Color &b, const float m)
{
    return (1.f - m) * a + m * b;
}

inline Color Red() { return {1.f, 0.f, 0.f, 1.f}; };
inline Color Green() { return {0.f, 1.f, 0.f, 1.f}; };
inline Color Blue() { return {0.f, 0.f, 1.f, 1.f}; };
inline Color Black() { return {0.f, 0.f, 0.f, 1.f}; };
inline Color White() { return {1.f, 1.f, 1.f, 1.f}; };