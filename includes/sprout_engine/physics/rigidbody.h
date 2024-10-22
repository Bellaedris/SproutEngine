//
// Created by arpradier on 17/10/2024.
//


#pragma once
#include <sprout_engine/SproutApp.h>
#include <sprout_engine/SproutApp.h>
#include <sprout_engine/transform.h>


class Rigidbody
{
protected:
    glm::vec3 m_position;
    glm::vec3 m_speed;
    glm::vec3 m_acceleration;
    glm::vec3 m_drag;
    glm::vec3 m_externalForce{0, 0, 0};

    float m_mass;

    bool isStatic{false};

public:
    Rigidbody();
    Rigidbody(const glm::vec3& position, float mass);
    Rigidbody(const glm::vec3& position, const glm::vec3& speed, const glm::vec3& acceleration, float mass);

    void updatePosition(float deltaTime, const glm::vec3& gravity);
    void addForce(const glm::vec3& force);
    void setStatic(bool p_isStatic);

    [[nodiscard]] glm::vec3 GetPosition() const { return m_position; };
    [[nodiscard]] glm::vec3 GetAcceleration() const { return m_acceleration; };
    [[nodiscard]] glm::vec3 GetSpeed() const { return m_speed; }
};