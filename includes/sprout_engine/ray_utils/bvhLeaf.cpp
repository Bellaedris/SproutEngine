//
// Created by arpradier on 30/09/2024.
//

#include "bvhLeaf.h"

BVHLeaf::BVHLeaf(const std::vector<Traceable*>& nodes)
    : m_nodes(nodes)
    , m_AABB(BoundingBox::empty())
{
    for(const auto& node : m_nodes)
        m_AABB = BoundingBox(m_AABB, node->getAABB());
}

bool BVHLeaf::hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const
{
    bool ret{};
    for(const auto& node : m_nodes)
        ret |= node->hit(r, p_t, hitInfo);

    return ret;
}

BoundingBox BVHLeaf::getAABB() const
{
    return m_AABB;
}

glm::vec3 BVHLeaf::getCentroid() const
{
    glm::vec3 ret;
    for(const auto& node : m_nodes)
        ret += node->getCentroid();

    return ret / static_cast<float>(m_nodes.size());
}

void BVHLeaf::drawAABB(Shader& s, int currentDepth, int maxDepth)
{
    m_AABB.draw(s);
}
