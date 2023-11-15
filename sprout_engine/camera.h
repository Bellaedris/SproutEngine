#pragma once
#include "glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "shader.h"

enum CAMERA_DIR
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct Plane
{
	glm::vec3 n;
	float d; // distance from origin to nearest point of the plane

	Plane() = default;
	Plane(const glm::vec3& p, const glm::vec3& norm) : n(glm::normalize(norm)), d(glm::dot(n, p)) {}

	inline float getSignedDistanceToPlane(const glm::vec3& p) const { return glm::dot(n, p) - d; };
};

struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};

const float CAMERA_SPEED = 5.f;
const float CAMERA_SENSITIVITY = .1f;

class Camera
{
protected:
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 up;
	glm::vec3 right;

	float pitch;
	float yaw;

	void update_dir();
public:
	Camera();
	Camera(const glm::vec3 &pos, const glm::vec3 &up, float pitch, float yaw);

	glm::mat4 view() const;
	inline glm::vec3 get_position() const { return pos; };
	Frustum get_frustum(float aspect, float fov, float znear, float zfar) const;

	void process_input(CAMERA_DIR direction, float delta_time);
	void process_mouse_movement(float xoffset, float yoffset);

	void draw_frustum(Shader& s, float aspect, float fov, float znear, float zfar) const;
};