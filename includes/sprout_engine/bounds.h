#pragma once
#include "camera.h"
#include "transform.h"

#include <vector>

class AABB
{
protected:
	glm::vec3 center;
	glm::vec3 extents;

public:
	AABB() = default;
	AABB(glm::vec3 min, glm::vec3 max) : center{ (max + min) * .5f }, extents{ max.x - center.x, max.y - center.y, max.z - center.z } {};
	AABB(const glm::vec3& c, float extentsX, float extentsY, float extentsZ) : center{ c }, extents{ extentsX, extentsY, extentsZ } {}

	std::vector<glm::vec3> getVertices() const;

	bool isOnOrForwardPlane(const Plane &p) const;

	bool isOnFrustum(const Frustum& f, const Transform &t) const;

	AABB transform(const Transform& t) const;
};