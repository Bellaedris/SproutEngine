//
// Created by arpradier on 17/09/2024.
//

#include "lambertian.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/random.hpp>
#include <sprout_engine/ray_utils/Traceables/traceable.h>

bool Lambertian::scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const
{
    glm::vec3 direction = p_hit.m_normal + glm::sphericalRand(1.f);

    if (glm::all(glm::epsilonEqual(direction, glm::vec3(0.f, 0.f, 0.f), 0.001f )))
        direction = p_hit.m_normal;

    p_rayScattered = Ray(p_hit.m_intersection + p_hit.m_normal * 0.0001f, direction);
    p_color = albedo;
    return true;
}
