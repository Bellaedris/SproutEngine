//
// Created by Bellaedris on 22/07/2024.
//

#include "ray.h"

Ray::Ray(const glm::vec3 &p_origin, const glm::vec3 &p_direction)
    : m_origin(p_origin)
    , m_direction(p_direction)
{}

glm::vec3 Ray::at(float t) const {
    return m_origin + t * m_direction;
}
