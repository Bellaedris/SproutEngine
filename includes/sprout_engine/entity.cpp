//
// Created by Bellaedris on 28/04/2024.
//

#include "entity.h"

#include <IMGUI/imgui.h>

Entity::Entity(const std::string &path, bool flip_uv)
    : m_model(path, flip_uv) {}

void Entity::drawInspector() const {
    ImGui::Text("I am alive");
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
