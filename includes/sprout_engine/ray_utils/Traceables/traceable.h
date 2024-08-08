//
// Created by Bellaedris on 04/08/2024.
//

#ifndef SPROUTENGINE_TRACEABLE_H
#define SPROUTENGINE_TRACEABLE_H

#include "sprout_engine/ray_utils/ray.h"

struct HitInfo
{
    glm::vec3 m_intersection;
    glm::vec3 m_normal;
    float m_distance;

    HitInfo(const glm::vec3& p_intersect, const glm::vec3& p_norm, float p_dist)
        : m_intersection(p_intersect), m_normal(p_norm), m_distance(p_dist) {}
};

class Traceable {
public:
    virtual bool hit(const Ray& r, float tMin, float tMax, HitInfo &hitInfo) const = 0;
};


#endif //SPROUTENGINE_TRACEABLE_H
