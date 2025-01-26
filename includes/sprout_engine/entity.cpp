//
// Created by Bellaedris on 28/04/2024.
//

#include "entity.h"

#include <IMGUI/imgui.h>

Entity::Entity(const std::string& path, const std::string& name, bool flip_uv)
    : m_model(path, flip_uv)
    , m_name(name)
{
    if(name.empty())
        m_name = path;
}

void Entity::UpdatePosition(const glm::vec3& position)
{
    m_transform.m_pos = position;
    m_transform.computeModelMatrix();
}

void Entity::drawInspector(Camera *camera)
{
    if(ImGui::TreeNode(m_name.c_str()))
    {
        static ImGuizmo::OPERATION currentOp(ImGuizmo::ROTATE);
        if (ImGui::IsKeyPressed(ImGuiKey_W))
            currentOp = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            currentOp = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            currentOp = ImGuizmo::SCALE;

        if (ImGui::RadioButton("Translate", currentOp == ImGuizmo::TRANSLATE))
            currentOp = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", currentOp == ImGuizmo::ROTATE))
            currentOp = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", currentOp == ImGuizmo::SCALE))
            currentOp = ImGuizmo::SCALE;

        ImGui::InputFloat3("Position", glm::value_ptr(m_transform.m_pos));
        ImGui::InputFloat3("Rotation", glm::value_ptr(m_transform.m_eulerRot));
        ImGui::InputFloat3("Scale", glm::value_ptr(m_transform.m_scale));

        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(m_transform.m_pos),
                                                glm::value_ptr(m_transform.m_eulerRot),
                                                glm::value_ptr(m_transform.m_scale),
                                                glm::value_ptr(m_transform.getModelMatrix()));

        ImGuizmo::Manipulate(glm::value_ptr(camera->view()),
                             glm::value_ptr(camera->projection()),
                             currentOp,
                             ImGuizmo::MODE::WORLD,
                             glm::value_ptr(m_transform.getModelMatrix()),
                             nullptr,
                             nullptr);

        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(m_transform.getModelMatrix()),
                                                glm::value_ptr(m_transform.m_pos),
                                                glm::value_ptr(m_transform.m_eulerRot),
                                                glm::value_ptr(m_transform.m_scale)
                                                );

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
