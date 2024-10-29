//
// Created by arpradier on 25/10/2024.
//

#include "physicsEntity.h"

PhysicsEntity::PhysicsEntity(const std::string& path, const std::string& name, bool flip_uv)
    : Entity(path, name, flip_uv)
{}

void PhysicsEntity::drawInspector()
{
    if(ImGui::TreeNode(m_name.c_str()))
    {
        if(ImGui::InputFloat3("Position", glm::value_ptr(m_transform.m_pos)))
        {
            m_transform.computeModelMatrix();
            m_rb.m_position = m_transform.m_pos;
        }
        if(ImGui::InputFloat3("Rotation", glm::value_ptr(m_transform.m_eulerRot)))
            m_transform.computeModelMatrix();
        if(ImGui::InputFloat3("Scale", glm::value_ptr(m_transform.m_scale)))
            m_transform.computeModelMatrix();

        ImGui::InputFloat("Mass", &m_rb.m_mass);
        ImGui::InputFloat3("Speed", glm::value_ptr(m_rb.m_speed));
        ImGui::InputFloat3("Acceleration", glm::value_ptr(m_rb.m_acceleration));
        ImGui::Checkbox("Static", &m_rb.isStatic);

        ImGui::TreePop();
    }
}
