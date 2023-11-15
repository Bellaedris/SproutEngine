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
	AABB(glm::vec3 min, glm::vec3 max);
	AABB(const glm::vec3& c, float extentsX, float extentsY, float extentsZ) : center{ c }, extents{ extentsX, extentsY, extentsZ } {}

	std::vector<glm::vec3> getVertices() const;
	bool isOnOrForwardPlane(const Plane &p) const;
	bool isOnFrustum(const Frustum& p, const Transform &transform) const;

	AABB transform(const Transform& t) const;
};