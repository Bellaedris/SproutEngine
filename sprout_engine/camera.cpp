#include "camera.h"

using namespace glm;

Camera::Camera() : dir(vec3(0., 0., -1.)), pitch(0.), yaw(0.)
{
	pos = vec3(0., 0., 0.);
	up = vec3(0., 1., 0.);

	right = normalize(cross(dir, up));
}

Camera::Camera(vec3 pos, vec3 up, float pitch, float yaw) : dir(vec3(0., 0., -1.)), pos(pos), up(up), pitch(pitch), yaw(yaw)
{
	right = normalize(cross(dir, up));
}

mat4 Camera::view()
{
	return lookAt(pos, pos + dir, up);
}

void Camera::process_input(CAMERA_DIR direction, float delta_time)
{
	float velocity = CAMERA_SPEED * delta_time;
	switch (direction)
	{
	case FORWARD:
		pos += dir * velocity;
		break;
	case BACKWARD:
		pos -= dir * velocity;
		break;
	case LEFT:
		pos -= right * velocity;
		break;
	case RIGHT:
		pos += right * velocity;
		break;
	}

}

void Camera::process_mouse_movement(float xoffset, float yoffset)
{
	xoffset = std::abs(xoffset) <= 1.f ? 0.f : xoffset;
	yoffset = std::abs(yoffset) <= 1.f ? 0.f : yoffset;

	xoffset *= CAMERA_SENSITIVITY;
	yoffset *= CAMERA_SENSITIVITY;

	yaw += xoffset;
	pitch += yoffset;

	// to avoid the lookAt matrix to flip 
	if (pitch > 89.f)
		pitch = 89.f;
	if (pitch < -89.f)
		pitch = -89.f;

	vec3 new_dir;
	new_dir.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
	new_dir.y = std::sin(radians(pitch));
	new_dir.z = std::sin(radians(yaw)) * std::cos(radians(pitch));

	dir = normalize(new_dir);
}