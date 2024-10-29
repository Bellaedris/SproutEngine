//
// Created by arpradier on 25/10/2024.
//


#pragma once
#include <sprout_engine/entity.h>

#include "rigidbody.h"


class PhysicsEntity : public Entity
{
public:
    Rigidbody m_rb;

    PhysicsEntity(const std::string& path, const std::string& name, bool flip_uv = false);

    void drawInspector() override;
};
