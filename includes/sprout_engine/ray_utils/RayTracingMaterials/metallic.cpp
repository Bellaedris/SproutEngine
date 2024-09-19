//
// Created by arpradier on 17/09/2024.
//

#include "metallic.h"

#include <glm/gtc/random.hpp>

bool Metallic::scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const
{
    // the vector is normalized so the fuzzing is independant from the size of the sphere (ray directions aren't normalized)
    glm::vec3 reflected = glm::normalize(glm::reflect(p_rayIn.getDirection(), p_hit.m_normal));
    reflected = reflected + (m_fuzz * glm::sphericalRand(1.f));
    p_rayScattered = Ray(p_hit.m_intersection + p_hit.m_normal * 0.0001f, reflected);
    p_color = m_albedo;
    // since we added a random fuzzing that could possibly make our reflected direction end up inside the sphere,
    // we just return if our reflect is indeed outside of the sphere. If not, we return false
    return glm::dot(p_rayScattered.getDirection(), p_hit.m_normal) > 0.f;
}
