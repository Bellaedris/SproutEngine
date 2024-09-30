//
// Created by arpradier on 30/09/2024.
//

#pragma once

#include "Traceables/traceable.h"

class BVHLeaf : public Traceable
{
private:
    std::vector<Traceable*> m_nodes;
    BoundingBox m_AABB;

public:
    BVHLeaf(const std::vector<Traceable*>& nodes);

    bool hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const override;

    BoundingBox getAABB() const override;
    glm::vec3 getCentroid() const override;
    void drawAABB(Shader& s, int currentDepth, int maxDepth) override;
};
