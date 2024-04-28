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

    float m_zNear;
    float m_zFar;
    float m_fov;
    float m_aspectRatio;

	float pitch;
	float yaw;

    glm::mat4 m_view;
    glm::mat4 m_projection;

    Frustum m_frustum;

	void update_dir();

public:
	Camera();

	Camera(const glm::vec3 &pos, const glm::vec3 &up, float pitch, float yaw, float p_znear, float p_zfar, float p_fov, float p_aspectRatio);

	glm::mat4 view() const;
    glm::mat4 projection() const;

	inline glm::vec3 get_position() const { return pos; };
    inline Frustum getFrustum() const { return m_frustum; };

    void setZNear(float mZNear);
    void setZFar(float mZFar);
    void setFov(float mFov);
    void setAspectRatio(float mAspectRatio);

    void updateView();
    void updateProjection();
    void updateFrustum();

	void process_input(CAMERA_DIR direction, float delta_time);

	void process_mouse_movement(float xoffset, float yoffset);

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