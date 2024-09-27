//
// Created by arpradier on 23/09/2024.
//


#pragma once
#include <vector>

#include "traceableManager.h"
#include "Traceables/box.h"
#include "Traceables/traceable.h"

class BVHNode : public Traceable
{
public:
    BVHNode(std::vector<Traceable*>& p_traceables, size_t p_first, size_t p_end);

    bool hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const override;
    BoundingBox getAABB() const override;
    glm::vec3 getCentroid() const override;

    void drawAABB(Shader& s, int currentDepth, int maxDepth) override;

private:
    Traceable* m_left{};
    Traceable* m_right{};

    unsigned int first{}; //! first primitive index
    unsigned int count{}; //! number of primitives

    BoundingBox m_aabb;

    void sortOnAxis(std::vector<Traceable*>& p_traceables, size_t p_begin, size_t p_end, int axis);
};