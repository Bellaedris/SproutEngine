//
// Created by arpradier on 03/10/2024.
//

#include "triangle.h"

Triangle::Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const std::shared_ptr<RaytracingMaterial>& mat)
    : a(a), b(b), c(c), m_material(mat)
{
    m_aabb =
        {
            {
                std::min(std::min(a.x, b.x), c.x),
                std::min(std::min(a.y, b.y), c.y),
                std::min(std::min(a.z, b.z), c.z),
            },
            {
                std::max(std::max(a.x, b.x), c.x),
                std::max(std::max(a.y, b.y), c.y),
                std::max(std::max(a.z, b.z), c.z),
            }
        };
}

bool Triangle::hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 ray_cross_e2 = cross(r.getDirection(), ac);
    float det = dot(ab, ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return {};    // This ray is parallel to this triangle.

    float inv_det = 1.0f / det;
    vec3 s = r.getOrigin() - a;
    float u = inv_det * dot(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return {};

    vec3 s_cross_e1 = cross(s, ab);
    float v = inv_det * dot(r.getDirection(), s_cross_e1);

    if (v < 0 || u + v > 1)
        return {};

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = inv_det * dot(ac, s_cross_e1);

    if (t > epsilon) // ray intersection
    {
        hitInfo.m_distance = t;
        hitInfo.m_intersection = r.at(t);
        hitInfo.m_mat = m_material;

        glm::vec3 norm = glm::cross(ac, ab);
        if (glm::dot(norm, r.getDirection()) > 0.f) {
            hitInfo.m_normal = -norm;
            hitInfo.m_frontFace = false;
        }
        // same side, we are outside
        else
        {
            hitInfo.m_normal = norm;
            hitInfo.m_frontFace = true;
        }
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

BoundingBox Triangle::getAABB() const
{
    return m_aabb;
}

glm::vec3 Triangle::getCentroid() const
{
    return (a + b + c) / 3.f;
}

void Triangle::drawAABB(Shader& s, int currentDepth, int maxDepth)
{
    m_aabb.draw(s);
}
