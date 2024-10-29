//
// Created by Bellaedris on 28/04/2024.
//

#include "bounds.h"

#include <glm/gtc/random.hpp>

std::vector <glm::vec3> AABB::getVertices() const {
    std::vector<glm::vec3> vertices(8);
    vertices[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
    vertices[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
    vertices[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
    vertices[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
    vertices[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
    vertices[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
    vertices[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
    vertices[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
    return vertices;
}

bool AABB::isOnOrForwardPlane(const Plane &p) const {
    float r = extents.x * std::abs(p.n.x) + extents.y * std::abs(p.n.y) + extents.z * std::abs(p.n.z);

    return -r <= p.getSignedDistanceToPlane(center);
}

bool AABB::isOnFrustum(const Frustum &f, const Transform &t) const {
    const AABB globalAABB = transform(t);

    return (
            globalAABB.isOnOrForwardPlane(f.bottomFace) &&
            globalAABB.isOnOrForwardPlane(f.topFace) &&

            globalAABB.isOnOrForwardPlane(f.leftFace) &&
            globalAABB.isOnOrForwardPlane(f.rightFace) &&

            globalAABB.isOnOrForwardPlane(f.nearFace) &&
            globalAABB.isOnOrForwardPlane(f.farFace)
    );
}

AABB AABB::transform(const Transform &t) const {
    //Get global scale thanks to our transform
    const glm::vec3 globalCenter{ t.getModelMatrix() * glm::vec4(center, 1.f) };

    // Scaled orientation
    const glm::vec3 right = t.getRight() * extents.x;
    const glm::vec3 up = t.getUp() * extents.y;
    const glm::vec3 forward = t.getForward() * extents.z;

    const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
                        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
                        std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

    const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
                        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
                        std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

    const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
                        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
                        std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

    return AABB(globalCenter, newIi, newIj, newIk);
}

BoundingBox::BoundingBox(const BoundingBox& p_lhs, const BoundingBox& p_rhs)
{
    m_pmin = glm::vec3(
        std::min(p_lhs.m_pmin.x, p_rhs.m_pmin.x),
        std::min(p_lhs.m_pmin.y, p_rhs.m_pmin.y),
        std::min(p_lhs.m_pmin.z, p_rhs.m_pmin.z)
        );

    m_pmax = glm::vec3(
        std::max(p_lhs.m_pmax.x, p_rhs.m_pmax.x),
        std::max(p_lhs.m_pmax.y, p_rhs.m_pmax.y),
        std::max(p_lhs.m_pmax.z, p_rhs.m_pmax.z)
        );

    color = {glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f)};
}

glm::vec3 BoundingBox::getCenter() const
{
    return (m_pmin + m_pmax) * 0.5f;
}

BoundingBox BoundingBox::empty()
{
    return {
        {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
        {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()},
        {glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f), glm::linearRand(0.f, 1.f)}
    };
}

bool BoundingBox::hit(const Ray& r, Interval<float> p_t) const
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    if (r.getInverseDir().x >= 0)
    {
        tmin = (m_pmin.x - r.getOrigin().x) * r.getInverseDir().x;
        tmax = (m_pmax.x - r.getOrigin().x) * r.getInverseDir().x;
    }
    else
    {
        tmin = (m_pmax.x - r.getOrigin().x) * r.getInverseDir().x;
        tmax = (m_pmin.x - r.getOrigin().x) * r.getInverseDir().x;
    }

    if (r.getInverseDir().y >= 0)
    {
        tymin = (m_pmin.y - r.getOrigin().y) * r.getInverseDir().y;
        tymax = (m_pmax.y - r.getOrigin().y) * r.getInverseDir().y;
    }
    else
    {
        tymin = (m_pmax.y - r.getOrigin().y) * r.getInverseDir().y;
        tymax = (m_pmin.y - r.getOrigin().y) * r.getInverseDir().y;
    }

    if (tmin > tymax || tymin > tmax)
        return false;

    tmin = std::max(tmin, tymin);
    tmax = std::min(tmax, tymax);

    if (r.getInverseDir().z >= 0)
    {
        tzmin = (m_pmin.z - r.getOrigin().z) * r.getInverseDir().z;
        tzmax = (m_pmax.z - r.getOrigin().z) * r.getInverseDir().z;
    }
    else
    {
        tzmin = (m_pmax.z - r.getOrigin().z) * r.getInverseDir().z;
        tzmax = (m_pmin.z - r.getOrigin().z) * r.getInverseDir().z;
    }

    // Check overlap between current interval and z-slab
    if (tmin > tzmax || tzmin > tmax)
        return false;

    tmin = std::max(tmin, tzmin);
    tmax = std::min(tmax, tzmax);

    return tmin < 0 ? p_t.contains(tmax) : p_t.contains(tmin);
}

std::array<glm::vec3, 8> BoundingBox::getVertices() const
{
    std::array<glm::vec3, 8> vertices;
    vertices[0] = { m_pmin.x, m_pmin.y, m_pmin.z };
    vertices[1] = { m_pmax.x, m_pmin.y, m_pmin.z };
    vertices[2] = { m_pmin.x, m_pmax.y, m_pmin.z };
    vertices[3] = { m_pmax.x, m_pmax.y, m_pmin.z };
    vertices[4] = { m_pmin.x, m_pmin.y, m_pmax.z };
    vertices[5] = { m_pmax.x, m_pmin.y, m_pmax.z };
    vertices[6] = { m_pmin.x, m_pmax.y, m_pmax.z };
    vertices[7] = { m_pmax.x, m_pmax.y, m_pmax.z };
    return vertices;
}

void BoundingBox::buildBuffer()
{
    // AABB buffers/vao
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &buffer);

    glBindVertexArray(vao);

    // buffer containing all vertices data
    auto aabb_bounds = getVertices();
    glm::vec3 bounds[24] =
    {
        // front face (connect the 4 front corners in a loop)
        aabb_bounds[0], aabb_bounds[1],
        aabb_bounds[1], aabb_bounds[3],
        aabb_bounds[3], aabb_bounds[2],
        aabb_bounds[2], aabb_bounds[0],

        // back face (connect the 4 back corners in a loop)
        aabb_bounds[4], aabb_bounds[5],
        aabb_bounds[5], aabb_bounds[7],
        aabb_bounds[7], aabb_bounds[6],
        aabb_bounds[6], aabb_bounds[4],

        // edges between front and back face
        aabb_bounds[0], aabb_bounds[4],
        aabb_bounds[1], aabb_bounds[5],
        aabb_bounds[3], aabb_bounds[7],
        aabb_bounds[2], aabb_bounds[6],
    };

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bounds), &bounds, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    m_isInitialized = true;
}

void BoundingBox::draw(Shader& s)
{
    if (!m_isInitialized)
        buildBuffer();

    s.use();

    s.uniform_data("color", color);

    // draw AABB
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
}