#pragma once
#include "glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

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
	glm::vec3 p;
	glm::vec3 n;
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

	
public:
	Camera();
	Camera(glm::vec3 pos, glm::vec3 up, float pitch, float yaw);

	glm::mat4 view();
	inline glm::vec3 get_position() const { return pos; };
	Frustum get_frustum(float aspect, float fov, float znear, float zfar);

	void process_input(CAMERA_DIR direction, float delta_time);
	void process_mouse_movement(float xoffset, float yoffset);
};