//
// Created by arpradier on 23/09/2024.
//

#pragma once

#include <vector>

#include "traceableManager.h"
#include "Traceables/box.h"
#include "Traceables/traceable.h"
#include "gpuMeshReader.h"

class BVHNode : public Traceable
{
public:
    BVHNode(std::vector<Traceable*>& p_traceables, size_t p_first, size_t p_end, int currentDepth,  int maxDepth);

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

struct Node
{
    glm::vec3 pmin;
    int left;
    glm::vec3 pmax;
    int right;

    [[nodiscard]] bool isLeaf() const {return left < 0;}
    [[nodiscard]] bool isInternal() const {return left > 0;}

};

class BVH
{
public:
    void build(std::vector<TriangleData> triangles);
    void intersect(const Ray& r, HitInfo& hit);
    int height();
protected:
    void precomputeBounds();
    void build(int index, int start, int end);
    BoundingBox nodeBounds(int start, int end) const;
    void sortOnAxis(size_t p_begin, size_t p_end, int axis);
    int height(int index);

    std::vector<Node> m_nodes;
    std::vector<TriangleData> m_triangles;
    std::vector<BoundingBox> m_bounds;
    int numberOfNodes;
};

