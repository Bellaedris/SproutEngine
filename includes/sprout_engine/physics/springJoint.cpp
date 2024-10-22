//
// Created by arpradier on 22/10/2024.
//

#include "springJoint.h"

SpringJoint::SpringJoint(Rigidbody* left, Rigidbody* right, float stiffness, float maxLength)
    : m_left(left)
    , m_right(right)
    , m_stiffness(stiffness)
    , m_maxLength(maxLength)
{}

void SpringJoint::applyForce()
{
    glm::vec3 ab = m_right->GetPosition() - m_left->GetPosition();
    glm::vec3 ba = m_left->GetPosition() - m_right->GetPosition();
    float length = glm::length(ab);
    float resistance = m_stiffness * (length - m_maxLength);

    m_left->addForce(resistance * glm::normalize(ab));
    m_right->addForce(resistance * glm::normalize(ba));
}
