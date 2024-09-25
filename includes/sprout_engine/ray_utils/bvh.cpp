//
// Created by arpradier on 23/09/2024.
//

#include "bvh.h"

#include <algorithm>

#include "RayTracingMaterials/lambertian.h"

BVHNode::BVHNode(std::vector<Traceable*>& p_traceables, size_t p_first, size_t p_end)
{
    // slice in the longest axis of the BVH
    // to do so, compute the AABB of the current iteration by combining all AABB available
    m_aabb = BoundingBox::empty();
    for(int i = p_first; i < p_end; i++)
        m_aabb = BoundingBox(m_aabb, p_traceables[i]->getAABB());

    float sizeX = m_aabb.m_pmax.x - m_aabb.m_pmin.x;
    float sizeY = m_aabb.m_pmax.y - m_aabb.m_pmin.y;
    float sizeZ = m_aabb.m_pmax.z - m_aabb.m_pmin.z;

    int axis;
    if (sizeX > sizeY)
        axis = sizeX > sizeZ ? 0 : 2;
    else
        axis = sizeY > sizeZ ? 1 : 2;

    size_t count = p_end - p_first;

    if (count == 1)
    {
        m_left = m_right = p_traceables[p_first];
    }
    else if (count == 2)
    {
        m_left = p_traceables[p_first];
        m_right = p_traceables[p_first + 1];
    }
    else
    {
        // sort the traceables by their position on the sorting axis, then send half the traceables left/right
        sortOnAxis(p_traceables, p_first, p_end, axis);

        size_t middle = p_first + count / 2;
        m_left = new BVHNode(p_traceables, p_first, middle);
        m_right = new BVHNode(p_traceables, middle, p_end);
    }

    m_box = Box(m_aabb.m_pmin, m_aabb.m_pmax, std::make_shared<Lambertian>(Color(1, 0, 0, 1)));
}

bool BVHNode::hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const
{
    if (!m_aabb.hit(r, p_t))
        return false;

    bool hit_left = m_left->hit(r, p_t, hitInfo);
    bool hit_right = m_right->hit(r, p_t, hitInfo);

    return hit_left || hit_right;
}

BoundingBox BVHNode::getAABB() const
{
    return m_aabb;
}

glm::vec3 BVHNode::getCentroid() const
{
    return m_aabb.getCenter();
}

void BVHNode::sortOnAxis(std::vector<Traceable*>& p_traceables, size_t p_begin, size_t p_end, int axis)
{
    switch (axis)
    {
    case 0:
        std::sort(p_traceables.begin() + p_begin, p_traceables.begin() + p_end, [](const Traceable* a, const Traceable* b)
        {
            return a->getCentroid().x < b->getCentroid().x;
        });
        break;
    case 1:
        std::sort(p_traceables.begin() + p_begin, p_traceables.begin() + p_end, [](const Traceable* a, const Traceable* b)
        {
            return a->getCentroid().y < b->getCentroid().y;
        });
        break;
    case 2:
        std::sort(p_traceables.begin() + p_begin, p_traceables.begin() + p_end, [](const Traceable* a, const Traceable* b)
        {
            return a->getCentroid().z < b->getCentroid().z;
        });
        break;
    }
}
