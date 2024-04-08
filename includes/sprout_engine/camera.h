#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sprout_engine/shader.h>

using namespace glm;

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

	void update_dir()
	{
		vec3 new_dir;
		new_dir.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
		new_dir.y = std::sin(radians(pitch));
		new_dir.z = std::sin(radians(yaw)) * std::cos(radians(pitch));

		dir = normalize(new_dir);
		right = normalize(cross(dir, up));
	};

public:
	Camera()
		: dir(vec3(0., 0., -1.)), pitch(0.), yaw(0.)
	{
		pos = vec3(0., 0., 0.);
		up = vec3(0., 1., 0.);

		right = normalize(cross(dir, up));
	};

	Camera(const glm::vec3 &pos, const glm::vec3 &up, float pitch, float yaw) : dir(vec3(0., 0., -1.)), pos(pos), up(up), pitch(pitch), yaw(yaw)
	{
		right = normalize(cross(dir, up));
		update_dir();
	};

	glm::mat4 view() const {
		return lookAt(pos, pos + dir, up);
	};

	inline glm::vec3 get_position() const { return pos; };

	Frustum get_frustum(float aspect, float fov, float znear, float zfar) const
	{
		Frustum frustum;
		float halfVSide = std::tan(glm::radians(fov) * .5f) * zfar; // find the half height of the far plane with trigo
		float halfHSide = halfVSide * aspect; // aspect = w / h
		vec3 farPlaneCenter = zfar * dir;

		frustum.farFace = { pos + farPlaneCenter, -dir };
		frustum.nearFace = { pos + znear * dir, dir };

		frustum.rightFace = { pos , cross(farPlaneCenter - right * halfHSide, up) };
		frustum.leftFace = { pos , cross(up, farPlaneCenter + right * halfHSide) };

		frustum.topFace = { pos , cross(right, farPlaneCenter - up * halfVSide) };
		frustum.bottomFace = { pos , cross(farPlaneCenter + up * halfVSide, right) };

		return frustum;
	};

	void process_input(CAMERA_DIR direction, float delta_time)
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
		case UP:
			pos += up * velocity;
			break;
		case DOWN:
			pos -= up * velocity;
			break;
		}

	};

	void process_mouse_movement(float xoffset, float yoffset)
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

		update_dir();
	};

	void draw_frustum(Shader& s, float aspect, float fov, float znear, float zfar) const
	{
		glm::mat4 inversev = glm::inverse(view());

		float halfVSide = std::tan(glm::radians(fov) * .5f); // frustum half height of the far plane with trigo
		float halfHSide = halfVSide * aspect; // aspect = w / h

		float xn = halfVSide * znear;
		float xf = halfVSide * zfar;
		float yn = halfHSide * znear;
		float yf = halfHSide * zfar;

		glm::vec4 f[8u] =
		{
			// near face
			{xn, yn, -znear, 1.f},
			{-xn, yn, -znear, 1.f},
			{xn, -yn, -znear, 1.f},
			{-xn, -yn, -znear , 1.f},

			// far face
			{xf, yf, -zfar, 1.f},
			{-xf, yf, -zfar , 1.f},
			{xf, -yf, -zfar , 1.f},
			{-xf, -yf, -zfar, 1.f},
		};

		// build and store the frustum corners
		glm::vec3 v[8];
		for (int i = 0; i < 8; i++)
		{
			glm::vec4 ff = inversev * f[i];
			v[i].x = ff.x / ff.w;
			v[i].y = ff.y / ff.w;
			v[i].z = ff.z / ff.w;
		}

		// store lines to draw
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glm::vec3 vertices[24] =
		{
			v[0], v[1],
			v[0], v[2],
			v[3], v[1],
			v[3], v[2],

			v[4], v[5],
			v[4], v[6],
			v[7], v[5],
			v[7], v[6],

			v[0], v[4],
			v[1], v[5],
			v[2], v[6],
			v[3], v[7],
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		s.use();

		s.uniform_data("color", 0.f, 1.f, 0.f, 0.f);

		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, 24);
		glBindVertexArray(0);
	};
};