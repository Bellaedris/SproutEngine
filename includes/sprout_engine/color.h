#pragma once

#include <glm/glm.hpp>

using Color = glm::vec4;

inline Color Red() { return {1.f, 0.f, 0.f, 1.f}; };
inline Color Green() { return {0.f, 1.f, 0.f, 1.f}; };
inline Color Blue() { return {0.f, 0.f, 1.f, 1.f}; };
inline Color Black() { return {0.f, 0.f, 0.f, 1.f}; };
inline Color White() { return {1.f, 1.f, 1.f, 1.f}; };