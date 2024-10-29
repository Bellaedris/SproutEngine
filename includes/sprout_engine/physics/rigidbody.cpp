//
// Created by arpradier on 17/10/2024.
//

#include "rigidbody.h"

Rigidbody::Rigidbody()
    : m_position({0, 0, 0})
    , m_speed({0, 0, 0})
    , m_acceleration({0, 0, 0})
    , m_drag({0, 0, 0})
    , m_mass(1)
{}

Rigidbody::Rigidbody(const glm::vec3& position, float mass)
    : m_position(position)
    , m_mass(mass)
    , m_speed({0, 0, 0})
    , m_acceleration({0, 0, 0})
    , m_drag({0, 0, 0})
{}

Rigidbody::Rigidbody(const glm::vec3& position, const glm::vec3& speed, const glm::vec3& acceleration, float mass)
    : m_position(position)
    , m_speed(speed)
    , m_acceleration(acceleration)
    , m_drag(glm::vec3(0))
    , m_mass(mass)
    {}

void Rigidbody::updatePosition(float deltaTime, const glm::vec3& gravity)
{
    if (isStatic)
        return;

    // euler semi-implicit
    glm::vec3 localGravity = gravity * m_mass;
    if (glm::length(m_speed) == 0.f)
        m_drag = {0.f, 0.f, 0.f};
    else
        m_drag = -0.3136f * glm::length(m_speed) * glm::length(m_speed) * glm::normalize(m_speed);
    m_acceleration = (localGravity + m_drag + m_externalForce) / m_mass;

    m_speed = m_speed + m_acceleration * deltaTime;
    m_position = m_position + m_speed * deltaTime;
}

void Rigidbody::addForce(const glm::vec3& force)
{
    m_externalForce += force;
}

void Rigidbody::setStatic(bool p_isStatic)
{
    isStatic = p_isStatic;
}
