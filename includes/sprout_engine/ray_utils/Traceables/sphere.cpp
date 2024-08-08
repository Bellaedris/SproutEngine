//
// Created by Bellaedris on 04/08/2024.
//

#include "sphere.h"

Sphere::Sphere(const glm::vec3 &p_center, float p_radius)
    : m_center(p_center)
    , m_radius(std::fmax(0.f, p_radius))
    {}

bool Sphere::hit(const Ray &r, float tMin, float tMax, HitInfo &hitInfo) const {
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
    if (root < tMin || root > tMax)
    {
        root = (h + sqrtDelta) / a;
        if (root < tMin || root > tMax)
            return false;
    }

    hitInfo.m_distance = root;
    hitInfo.m_intersection = r.at(root);
    hitInfo.m_normal = (hitInfo.m_intersection - m_center) / m_radius;

    return true;
}
