//
// Created by Bellaedris on 04/08/2024.
//

#include "sphere.h"

Sphere::Sphere(const glm::vec3 &p_center, float p_radius, const std::shared_ptr<RaytracingMaterial>& p_material)
    : m_center(p_center)
    , m_radius(std::fmax(0.f, p_radius))
    , m_material(p_material)
    , m_boundingBox(BoundingBox::empty())
{
    glm::vec3 radVec = {m_radius, m_radius, m_radius};
    m_boundingBox = {m_center - radVec, m_center + radVec};
}

bool Sphere::hit(const Ray &r, Interval<float> p_t, HitInfo &hitInfo) const {
    glm::vec3 oc = m_center - r.getOrigin();

    float a = glm::dot(r.getDirection(), r.getDirection());
    float h = glm::dot(r.getDirection(), oc);
    float c = glm::dot(oc, oc) - m_radius * m_radius;

    float delta = h * h - a * c;
    if (delta < 0.f)
        return false;

    // find nearest result in range
    float sqrtDelta = std::sqrtf(delta);
    float root = (h - sqrtDelta) / a;
    if (!p_t.surrounds(root))
    {
        root = (h + sqrtDelta) / a;
        if (!p_t.surrounds(root))
            return false;
    }

    hitInfo.m_distance = root;
    hitInfo.m_intersection = r.at(root);
    hitInfo.m_mat = m_material;

    glm::vec3 norm = (hitInfo.m_intersection - m_center) / m_radius;
    // opposite side, we are inside the sphere
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

BoundingBox Sphere::getAABB() const
{
    return m_boundingBox;
}

glm::vec3 Sphere::getCentroid() const
{
    return m_center;
}
