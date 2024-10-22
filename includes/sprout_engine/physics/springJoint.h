//
// Created by arpradier on 22/10/2024.
//


#pragma once
#include "joints.h"
#include "rigidbody.h"

class SpringJoint : public Joint
{
protected:
    Rigidbody* m_left;
    Rigidbody* m_right;

    float m_stiffness;
    float m_maxLength;

public:
    SpringJoint() = default;
    SpringJoint(Rigidbody* left, Rigidbody* right, float stiffness, float maxLength);
    void applyForce() override;
};
