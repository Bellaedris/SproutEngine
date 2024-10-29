//
// Created by arpradier on 03/10/2024.
//


#pragma once
#include "traceable.h"


class Triangle : public Traceable
{
public:
    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const std::shared_ptr<RaytracingMaterial>& mat);
    bool hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const override;

    BoundingBox getAABB() const override;
    glm::vec3 getCentroid() const override;
    void drawAABB(Shader& s, int currentDepth, int maxDepth) override;

private:
    glm::vec3 a, b, c;
    BoundingBox m_aabb;

    std::shared_ptr<RaytracingMaterial> m_material;
};
