#pragma once
#include "camera.h"
#include "transform.h"

#include <vector>
#include <array>

#include "interval.h"
#include "ray_utils/ray.h"

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

class BoundingBox
{
private:
	unsigned int buffer;
	unsigned int vao;
	glm::vec3 color;
	bool m_isInitialized{};
    glm::vec3 m_center;

public:
	glm::vec3 m_pmin{};
	glm::vec3 m_pmax{};

    BoundingBox() = default;
    BoundingBox(const glm::vec3& pmin, const glm::vec3& pmax, const glm::vec3& color = glm::vec3(1.f, 1.f, 1.f))
        : m_pmin(pmin), m_pmax(pmax), color(color), m_center((pmin + pmax) * 0.5f) {}

    BoundingBox(const BoundingBox& p_lhs, const BoundingBox& p_rhs);

    BoundingBox& Include(const BoundingBox& other);

    glm::vec3 getCenter() const;

    static BoundingBox empty();

    bool hit(const Ray& r, Interval<float> p_t) const;

	std::array<glm::vec3, 8> getVertices() const;
	void buildBuffer();
	void draw(Shader& s);
};