//
// Created by Bellaedris on 04/08/2024.
//

#pragma once

#include "traceable.h"

class Sphere : public Traceable
{
public:
    Sphere(const glm::vec3& p_center, float p_radius, const std::shared_ptr<RaytracingMaterial>& p_material);

    bool hit(const Ray &r, Interval<float> p_t, HitInfo &hitInfo) const override;
    BoundingBox getAABB() const override;
    glm::vec3 getCentroid() const override;

private:
    glm::vec3 m_center;
    float m_radius;
    std::shared_ptr<RaytracingMaterial> m_material;

    BoundingBox m_boundingBox;
};

