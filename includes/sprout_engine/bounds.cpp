//
// Created by Bellaedris on 28/04/2024.
//

#include "bounds.h"

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
