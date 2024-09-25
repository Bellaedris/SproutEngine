//
// Created by Bellaedris on 04/08/2024.
//

#ifndef SPROUTENGINE_TRACEABLE_H
#define SPROUTENGINE_TRACEABLE_H

#include <memory>
#include <sprout_engine/bounds.h>

#include "sprout_engine/ray_utils/ray.h"
#include "sprout_engine/interval.h"

class RaytracingMaterial;

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
    //! a material representing how the surface reponds to light
    std::shared_ptr<RaytracingMaterial> m_mat;
};

class Traceable {
public:
    virtual bool hit(const Ray& r, Interval<float> p_t, HitInfo &hitInfo) const = 0;
    virtual BoundingBox getAABB() const = 0;
    virtual glm::vec3 getCentroid() const = 0;
};


#endif //SPROUTENGINE_TRACEABLE_H
