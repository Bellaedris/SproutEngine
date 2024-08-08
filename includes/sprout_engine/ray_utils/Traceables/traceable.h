//
// Created by Bellaedris on 04/08/2024.
//

#ifndef SPROUTENGINE_TRACEABLE_H
#define SPROUTENGINE_TRACEABLE_H

#include "sprout_engine/ray_utils/ray.h"
#include "sprout_engine/interval.h"

struct HitInfo
{
    //! intersection point
    glm::vec3 m_intersection{};
    //! normal at the intersection point
    glm::vec3 m_normal{};
    //! distance at which the intersection happened
    float m_distance{};
    //! true if front, false otherwise
    bool m_frontFace{};
};

class Traceable {
public:
    virtual bool hit(const Ray& r, Interval<float> p_t, HitInfo &hitInfo) const = 0;
};


#endif //SPROUTENGINE_TRACEABLE_H
