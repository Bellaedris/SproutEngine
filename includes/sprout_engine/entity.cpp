//
// Created by Bellaedris on 28/04/2024.
//

#include "entity.h"

#include <IMGUI/imgui.h>

Entity::Entity(const std::string& path, const std::string& name, bool flip_uv)
    : m_model(path, flip_uv)
    , m_name(name)
{}

void Entity::UpdatePosition(const glm::vec3& position)
{
    m_transform.m_pos = position;
    m_transform.computeModelMatrix();
}

void Entity::drawInspector() {
    if(ImGui::TreeNode(m_name.c_str()))
    {
        if(ImGui::InputFloat3("Position", glm::value_ptr(m_transform.m_pos)))
        {
            m_transform.computeModelMatrix();
        }
        if(ImGui::InputFloat3("Rotation", glm::value_ptr(m_transform.m_eulerRot)))
        {
            m_transform.computeModelMatrix();
        }
        if(ImGui::InputFloat3("Scale", glm::value_ptr(m_transform.m_scale)))
        {
            m_transform.computeModelMatrix();
        }

        ImGui::TreePop();
    }
}

void Entity::draw(Shader &p_shader, const Frustum& p_frustum, const Transform& p_transform) {
    m_model.draw(p_shader, p_frustum, p_transform);
}

const Transform &Entity::getTransform() const {
    return m_transform;
}

const Model &Entity::getModel() const {
    return m_model;
}

void Entity::draw(Shader &p_shader) {
    m_model.draw(p_shader);
}
