//
// Created by arpradier on 23/09/2024.
//


#pragma once
#include "traceable.h"


class Box : public Traceable
{
public:
    Box() = default;
    Box(const glm::vec3& p_pmin, const glm::vec3& p_pmax, const std::shared_ptr<RaytracingMaterial>& p_mat);

    bool hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const override;
    BoundingBox getAABB() const override;
    glm::vec3 getCentroid() const override;

protected:
    glm::vec3 m_pmin{}; //! min points of the bounding box
    glm::vec3 m_pmax{}; //! max point of the bounding box
    std::shared_ptr<RaytracingMaterial> m_material;
};
