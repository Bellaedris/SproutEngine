//
// Created by arpradier on 17/10/2024.
//

#include "physicsSolver.h"

#include <chrono>

PhysicsSolver::PhysicsSolver(const std::vector<PhysicsEntity*>& rigidbodies, const glm::vec3& gravity)
    : m_physicsEntities(rigidbodies)
    , m_gravity(gravity) {}

void PhysicsSolver::SetRunningState(bool state)
{
    isRunning.store(state);
}

[[noreturn]] void PhysicsSolver::run()
{
    auto previousTime = std::chrono::high_resolution_clock::now();
    double accumulatedTime = 0.;

    while(isRunning.load())
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - previousTime;
        previousTime = currentTime;

        accumulatedTime += elapsedTime.count();

        // at the beggining of the physics frame, if we have waited more
        // than delta time, update physics using elapsed time.
        // Here, we decided to use the total delta even if it's above our deltaTime,
        // so we only call once and "catch up" on the time skipped.
        if(accumulatedTime >= m_deltaTime)
        {
            // once
            solve(accumulatedTime);
            accumulatedTime = 0;
        }

        // eventually, sleep for a short duration?
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void PhysicsSolver::solve(float deltaTime)
{
    m_mutex.lock();
    for (auto& entity : m_physicsEntities)
    {
        entity->m_rb.updatePosition(deltaTime, m_gravity);
        entity->UpdatePosition(entity->m_rb.m_position);
    }
    m_mutex.unlock();
    for(auto& joint : m_joints)
    {
        joint->applyForce();
    }
}

void PhysicsSolver::addJoint(Joint* j)
{
    m_joints.push_back(j);
}
