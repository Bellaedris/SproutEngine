//
// Created by arpradier on 23/09/2024.
//

#include "box.h"

Box::Box(const glm::vec3& p_pmin, const glm::vec3& p_pmax, const std::shared_ptr<RaytracingMaterial>& p_mat)
    : m_pmin(p_pmin)
    , m_pmax(p_pmax)
    , m_material(p_mat)
{
    m_boundingBox = BoundingBox(p_pmin, p_pmax, glm::vec3(1, 0, 0));
}

bool Box::hit(const Ray& r, Interval<float> p_t, HitInfo& hitInfo) const
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    glm::vec3 norm;
    if (r.getDirection().x >= 0)
    {
        tmin = (m_pmin.x - r.getOrigin().x) * r.getInverseDir().x;
        tmax = (m_pmax.x - r.getOrigin().x) * r.getInverseDir().x;
        norm = glm::vec3(-1, 0, 0);
    }
    else
    {
        tmin = (m_pmax.x - r.getOrigin().x) * r.getInverseDir().x;
        tmax = (m_pmin.x - r.getOrigin().x) * r.getInverseDir().x;
        norm = glm::vec3(1, 0, 0);
    }

    if (r.getDirection().y >= 0)
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

    if (tymin > tmin)
    {
        tmin = tymin;
        norm = glm::vec3(0, -1, 0); // Update normal for Y-axis hit
    }
    if (tymax < tmax)
    {
        tmax = tymax;
        norm = glm::vec3(0, 1, 0);
    }

    if (r.getDirection().z >= 0)
    {
        tzmin = (m_pmin.z - r.getOrigin().z) * r.getInverseDir().z;
        tzmax = (m_pmax.z - r.getOrigin().z) * r.getInverseDir().z;
    }
    else
    {
        tzmin = (m_pmax.z - r.getOrigin().z) * r.getInverseDir().z;
        tzmax = (m_pmin.z - r.getOrigin().z) * r.getInverseDir().z;
    }

    if (tmin > tzmax || tzmin > tmax)
        return false;

    if (tzmin > tmin)
    {
        tmin = tzmin;
        norm = glm::vec3(0, 0, -1); // Update normal for Z-axis hit
    }
    if (tzmax < tmax)
    {
        tmax = tzmax;
        norm = glm::vec3(0, 0, 1); // Update normal for Z-axis hit
    }

    if (p_t.contains(tmin))
    {
        hitInfo.m_distance = tmin;
        hitInfo.m_intersection = r.at(tmin);
        hitInfo.m_normal = norm;
        hitInfo.m_mat = m_material;

        hitInfo.m_frontFace = glm::dot(r.getDirection(), norm) < 0;

        // If not front face, reverse the normal direction
        if (!hitInfo.m_frontFace)
            hitInfo.m_normal = -hitInfo.m_normal;

        return true;
    }

    return false;
}

BoundingBox Box::getAABB() const
{
    return m_boundingBox;
}

glm::vec3 Box::getCentroid() const
{
    return (m_pmin + m_pmax) / 2.f;
}

void Box::drawAABB(Shader& s, int currentDepth, int maxDepth)
{
    m_boundingBox.draw(s);
}
