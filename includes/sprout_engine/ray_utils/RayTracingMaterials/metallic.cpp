//
// Created by arpradier on 17/09/2024.
//

#include "metallic.h"

bool Metallic::scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const
{
    glm::vec3 reflected = glm::reflect(p_rayIn.getDirection(), p_hit.m_normal);
    p_rayScattered = Ray(p_hit.m_intersection + p_hit.m_normal * 0.0001f, reflected);
    p_color = albedo;
    return true;
}
