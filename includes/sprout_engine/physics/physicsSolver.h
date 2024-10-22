//
// Created by arpradier on 17/10/2024.
//


#pragma once
#include <mutex>
#include <vector>

#include "rigidbody.h"
#include "joints.h"


class PhysicsSolver {
protected:
    //TODO double buffering of positions?
    std::vector<Rigidbody*> m_rigidbodies;
    std::vector<Joint*> m_joints;

    float m_deltaTime = 0.02f;

    glm::vec3 m_gravity;

public:
    PhysicsSolver() = default;

    PhysicsSolver(const PhysicsSolver& other)
        : m_rigidbodies(other.m_rigidbodies),
          m_joints(other.m_joints),
          m_deltaTime(other.m_deltaTime),
          m_gravity(other.m_gravity)
    {
    }

    PhysicsSolver(PhysicsSolver&& other) noexcept
        : m_rigidbodies(std::move(other.m_rigidbodies)),
          m_joints(std::move(other.m_joints)),
          m_deltaTime(other.m_deltaTime),
          m_gravity(std::move(other.m_gravity))
    {
    }

    PhysicsSolver& operator=(const PhysicsSolver& other)
    {
        if (this == &other)
            return *this;
        m_rigidbodies = other.m_rigidbodies;
        m_joints = other.m_joints;
        m_deltaTime = other.m_deltaTime;
        m_gravity = other.m_gravity;
        return *this;
    }

    PhysicsSolver& operator=(PhysicsSolver&& other) noexcept
    {
        if (this == &other)
            return *this;
        m_rigidbodies = std::move(other.m_rigidbodies);
        m_joints = std::move(other.m_joints);
        m_deltaTime = other.m_deltaTime;
        m_gravity = std::move(other.m_gravity);
        return *this;
    }

protected:
    std::mutex m_mutex;

public:
    PhysicsSolver(const std::vector<Rigidbody*>& rigidbodies, const glm::vec3& gravity);

    void run();
    void solve(float deltaTime);
    void addJoint(Joint* j);
};
