//
// Created by arpradier on 23/09/2024.
//

#include "bvhNode.h"

#include <algorithm>
#include <chrono>

#include "bvhLeaf.h"
#include "RayTracingMaterials/lambertian.h"

#pragma region TraceableBVH
BVHNode::BVHNode(std::vector<Traceable*>& p_traceables, size_t p_first, size_t p_end, int currentDepth, int maxDepth)
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
    else if (currentDepth > maxDepth)
    {
        m_left = p_traceables[p_first];
        m_right = p_traceables[p_first + 1];

        size_t middle = p_first + count / 2;
        m_left = new BVHLeaf({p_traceables.begin() + p_first, p_traceables.begin() + middle});
        m_right = new BVHLeaf({p_traceables.begin() + middle, p_traceables.begin() + p_end});
    }
    else
    {
        // sort the traceables by their position on the sorting axis, then send half the traceables left/right
        sortOnAxis(p_traceables, p_first, p_end, axis);

        size_t middle = p_first + count / 2;
        m_left = new BVHNode(p_traceables, p_first, middle, currentDepth + 1, maxDepth);
        m_right = new BVHNode(p_traceables, middle, p_end, currentDepth + 1, maxDepth);
    }
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

void BVHNode::drawAABB(Shader& s, int currentDepth, int maxDepth)
{
    m_aabb.draw(s);

    if (currentDepth < maxDepth)
    {
        m_left->drawAABB(s, currentDepth + 1, maxDepth);
        m_right->drawAABB(s, currentDepth + 1, maxDepth);
    }
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
#pragma endregion

#pragma region GPUFriendlyBVH
void BVH::build(std::vector<TriangleData> triangles)
{
    auto start = std::chrono::high_resolution_clock::now();

    m_triangles = triangles;
    m_bounds.resize(triangles.size());
    m_nodes.resize(triangles.size() * 2);
    numberOfNodes = 0;
    build(0, 0, triangles.size());

    auto bvhEnd = std::chrono::high_resolution_clock::now();

    std::cout << "BVH built in " << std::chrono::duration_cast<std::chrono::milliseconds>(bvhEnd - start).count() << "ms" << std::endl;
}

void BVH::build(int index, int start, int end)
{
    BoundingBox aabb = nodeBounds(start, end);

    // create a leaf node
    if (end - start < 4)
    {
        m_nodes[index] = {aabb.m_pmin, -start, aabb.m_pmax, -end};
        return;
    }

    float sizeX = aabb.m_pmax.x - aabb.m_pmin.x;
    float sizeY = aabb.m_pmax.y - aabb.m_pmin.y;
    float sizeZ = aabb.m_pmax.z - aabb.m_pmin.z;

    int axis;
    if (sizeX > sizeY)
        axis = sizeX > sizeZ ? 0 : 2;
    else
        axis = sizeY > sizeZ ? 1 : 2;

    sortOnAxis(start, end, axis);
    int middle = (start + end) / 2;

    int left = numberOfNodes++;
    int right = numberOfNodes++;

    m_nodes[index] = {aabb.m_pmin, left, aabb.m_pmax, right};

    build(left, start, middle);
    build(right, middle, end);
}

BoundingBox BVH::nodeBounds(int start, int end) const
{
    BoundingBox aabb = m_bounds[start];
    for(int i = start + 1; i < end; i++)
        aabb.Include(m_bounds[i]);

    return aabb;
}

void BVH::sortOnAxis(size_t p_begin, size_t p_end, int axis)
{
    int middle = (p_begin + p_end) / 2;
    auto comparator = [axis](const TriangleData& a, const TriangleData& b) {
        switch (axis) {
            case 0: return a.bounds().getCenter().x < b.bounds().getCenter().x;
            case 1: return a.bounds().getCenter().y < b.bounds().getCenter().y;
            case 2: return a.bounds().getCenter().z < b.bounds().getCenter().z;
        }
        return false;
    };

    std::nth_element(m_triangles.begin() + p_begin,
                     m_triangles.begin() + middle,
                     m_triangles.begin() + p_end,
                     comparator);
}

void BVH::intersect(const Ray &r, HitInfo &hit)
{
    int stack[64];
    int top = 0;


}

int BVH::height()
{
    int root = 0;
    return height(root);
}

int BVH::height(int index)
{
    const Node& n = m_nodes[index];
    if (n.isInternal())
        return 1 + std::max(height(n.left), height(n.right));
    return 0;
}

void BVH::precomputeBounds()
{
    for(int i = 0; i < m_triangles.size(); i++)
        m_bounds[i] = m_triangles[i].bounds();
}

#pragma endregion