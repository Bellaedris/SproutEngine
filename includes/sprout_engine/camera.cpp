//
// Created by Bellaedris on 28/04/2024.
//

#include "camera.h"
#include "imgui/imgui.h"

void Camera::update_dir() {
    vec3 new_dir;
    float yawRad = radians(yaw);
    float pitchRad = radians(pitch);
    new_dir.x = std::cos(yawRad) * std::cos(pitchRad);
    new_dir.y = std::sin(pitchRad);
    new_dir.z = std::sin(yawRad) * std::cos(pitchRad);

    dir = normalize(new_dir);
    right = normalize(cross(dir, worldUp));
    up = normalize(cross(right, dir));

    updateView();
}

Camera::Camera()
        : dir(vec3(0., 0., -1.)), pitch(0.), yaw(0.)
{
    pos = vec3(0., 0., 0.);
    worldUp = vec3(0., 1., 0.);

    right = normalize(cross(dir, worldUp));
}

Camera::Camera(const vec3 &pos, const vec3 &up, float pitch, float yaw, float p_znear, float p_zfar, float p_fov,
               float p_aspectRatio)
        : dir(vec3(0., 0., -1.)), pos(pos), worldUp(up), up(up), pitch(pitch), yaw(yaw), m_zNear(p_znear), m_zFar(p_zfar), m_fov(p_fov), m_aspectRatio(p_aspectRatio)
{
    right = normalize(cross(dir, up));
    update_dir();

    updateView();
    updateProjection();
    updateFrustum();
}

glm::mat4 Camera::view() const {
    return m_view;
}

glm::mat4 Camera::projection() const {
    return m_projection;
}

void Camera::setZNear(float mZNear) {
    m_zNear = mZNear;
    updateProjection();
}

void Camera::setZFar(float mZFar) {
    m_zFar = mZFar;
    updateProjection();
}

void Camera::setFov(float mFov) {
    m_fov = mFov;
    updateProjection();
}

void Camera::setAspectRatio(float mAspectRatio) {
    m_aspectRatio = mAspectRatio;
    updateProjection();
}

void Camera::updateView() {
    m_view = glm::lookAt(pos, pos + dir, up);
    updateFrustum();
}

void Camera::updateProjection() {
    float itan= 1 / tanf(glm::radians(m_fov) * 0.5f);
    float id= 1 / (m_zNear - m_zFar);

    m_projection = {
            itan / m_aspectRatio, 0, 0, 0,
            0, itan, 0, 0,
            0, 0, (m_zFar + m_zNear) * id, -1,
            0, 0, 2.f * m_zFar * m_zNear * id, 0
    };
    updateFrustum();
}

void Camera::updateFrustum() {
    Frustum frustum;
    float halfVSide = std::tan(glm::radians(m_fov) * .5f) * m_zFar; // find the half height of the far plane with trigo
    float halfHSide = halfVSide * m_aspectRatio; // aspect = w / h
    vec3 farPlaneCenter = m_zFar * dir;

    frustum.farFace = { pos + farPlaneCenter, -dir };
    frustum.nearFace = { pos + m_zNear * dir, dir };

    frustum.rightFace = { pos , cross(farPlaneCenter - right * halfHSide, up) };
    frustum.leftFace = { pos , cross(up, farPlaneCenter + right * halfHSide) };

    frustum.topFace = { pos , cross(right, farPlaneCenter - up * halfVSide) };
    frustum.bottomFace = { pos , cross(farPlaneCenter + up * halfVSide, right) };

    m_frustum = frustum;
}

void Camera::process_input(CAMERA_DIR direction, float delta_time) {
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
    update_dir();
}

void Camera::process_mouse_movement(float xoffset, float yoffset) {
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
}

glm::vec3 Camera::getDir() const
{
    return dir;
}

glm::vec3 Camera::getUp() const
{
    return up;
}

glm::vec3 Camera::getRight() const
{
    return right;
}

void Camera::drawInspector() {
    if(ImGui::TreeNode("Camera"))
    {
        if(ImGui::InputFloat3("Position", glm::value_ptr(pos)))
        {
            update_dir();
        }
        if (ImGui::InputFloat("Pitch", &pitch))
        {
            update_dir();
        }
        if (ImGui::InputFloat("Yaw", &yaw))
        {
            update_dir();
        }
        if (ImGui::InputFloat("FoV", &m_fov))
        {
            updateProjection();
        }
        ImGui::TreePop();
    }
}
