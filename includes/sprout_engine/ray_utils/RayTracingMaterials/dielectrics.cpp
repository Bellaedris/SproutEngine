//
// Created by arpradier on 19/09/2024.
//

#include "dielectrics.h"

#include <sprout_engine/utils.h>

bool Dielectric::scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const
{
    p_color = Color(1., 1., 1., 1.);

    // if we hit a backface (inside the sphere), the refractive index is the inverse
    const float ri = p_hit.m_frontFace ? 1.f / m_refractiveIndex : m_refractiveIndex;

    float cosTheta = glm::dot(glm::normalize(p_rayIn.getDirection()), p_hit.m_normal);
    float sinTheta = std::sqrtf(1 - cosTheta * cosTheta);

    bool canRefract = ri * sinTheta <= 1.0;

    // this can happen and has no solution. If we are in that case, we should diffract instead of refract
    if (canRefract || reflectance(cosTheta) <= Utils::random01())
    {
        const glm::vec3 refracted = glm::refract(
            glm::normalize(p_rayIn.getDirection()),
            p_hit.m_normal,
            ri
        );

        p_rayScattered = Ray(p_hit.m_intersection, refracted);
    }
    else
    {
        const glm::vec3 diffracted = glm::normalize(glm::reflect(p_rayIn.getDirection(), p_hit.m_normal));

        p_rayScattered = Ray(p_hit.m_intersection, diffracted);
    }

    return true;
}

double Dielectric::reflectance(float p_cosine) const
{
    float r0 = (1 - m_refractiveIndex) / (1 + m_refractiveIndex);
    r0 *= r0;
    return r0 + (1 - r0) * std::pow((1 - p_cosine), 5);
}
