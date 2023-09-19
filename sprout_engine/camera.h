#pragma once
#include "glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

enum CAMERA_DIR
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
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

	void process_input(CAMERA_DIR direction, float delta_time);
	void process_mouse_movement(float xoffset, float yoffset);
};