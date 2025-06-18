//
// Created by Bellaedris on 28/04/2024.
//

#include "entity.h"

#include <IMGUI/imgui.h>

Entity::Entity(const std::string& path, const std::string& name, bool flip_uv)
    : m_model(new Model(path, flip_uv))
    , m_transform(new Transform())
    , m_name(name)
    , m_bIsRootNode(false)
{
    if(name.empty())
    {
        size_t nameBegin = path.find_last_of("/\\");
        m_name = path.substr(nameBegin + 1, path.size() - nameBegin);
    }
}

void Entity::SetPosition(const glm::vec3& position)
{
    m_transform->m_pos = position;
    m_transform->computeModelMatrix();
}

void Entity::drawInspector(Camera *camera)
{
    if(!m_bIsRootNode)
    {
        ImGui::Text(m_name.c_str());
        ImGui::Separator();

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

        ImGui::InputFloat3("Position", glm::value_ptr(m_transform->m_pos));
        ImGui::InputFloat3("Rotation", glm::value_ptr(m_transform->m_eulerRot));
        ImGui::InputFloat3("Scale", glm::value_ptr(m_transform->m_scale));

        ImGuizmo::RecomposeMatrixFromComponents(
                glm::value_ptr(m_transform->m_pos),
                glm::value_ptr(m_transform->m_eulerRot),
                glm::value_ptr(m_transform->m_scale),
                glm::value_ptr(m_transform->getModelMatrix()));

        ImGuizmo::Manipulate(
                glm::value_ptr(camera->view()),
                glm::value_ptr(camera->projection()),
                currentOp,
                ImGuizmo::MODE::WORLD,
                glm::value_ptr(m_transform->getModelMatrix()),
                nullptr,
                nullptr
        );

        ImGuizmo::DecomposeMatrixToComponents(
                glm::value_ptr(m_transform->getModelMatrix()),
                glm::value_ptr(m_transform->m_pos),
                glm::value_ptr(m_transform->m_eulerRot),
                glm::value_ptr(m_transform->m_scale)
        );
    }
}

void Entity::drawHierarchy(Entity &selectedEntity)
{
    if(!m_bIsRootNode)
    {
        bool isSelected = *this == selectedEntity;
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (isSelected)
            node_flags |= ImGuiTreeNodeFlags_Selected;

        if(m_children.empty())
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            ImGui::TreeNodeEx(m_name.c_str(), node_flags);
            if(ImGui::IsItemClicked())
                selectedEntity = *this;
        }
        else
        {
            bool nodeOpen = ImGui::TreeNodeEx(m_name.c_str(), node_flags);
            if(ImGui::IsItemClicked())
                selectedEntity = *this;
            if(nodeOpen)
            {
                for (auto &child: m_children)
                    child->drawHierarchy(selectedEntity);

                ImGui::TreePop();
            }
        }

    }
    // the root node does not contain anything and should just display its children
    else
        for(auto& child : m_children)
            child->drawHierarchy(selectedEntity);
}


void Entity::draw(Shader &p_shader, const Frustum& p_frustum, const Transform& p_transform) {
    m_model->draw(p_shader, p_frustum, p_transform);
}

const Transform &Entity::getTransform() const {
    return *m_transform;
}

void Entity::draw(Shader &p_shader) {
    m_model->draw(p_shader);
}

void Entity::AddChild(const std::string &path, const std::string &name, bool flip_uv)
{
    m_children.push_back(std::make_unique<Entity>(path, name, flip_uv));
    m_children.back()->m_parent = this;
    m_children.back()->m_depth = m_depth + 1;
}

void Entity::AddChild(std::shared_ptr<Entity> e)
{
    m_children.push_back(std::move(e));
}

void Entity::UpdateHierarchy()
{
    // only update yourself and your children if you changed
    if(m_transform->isDirty())
    {
        ForceUpdateHierarchy();
        return;
    }

    // otherwise recurse and check your children
    for(auto& child : m_children)
    {
        child->UpdateHierarchy();
    }
}

void Entity::ForceUpdateHierarchy()
{
    if(m_parent)
        m_transform->computeModelMatrix(m_parent->m_transform->getModelMatrix());
    else
        m_transform->computeModelMatrix();

    // since we changed our parent, we have to force update the children
    for(auto& child : m_children)
    {
        child->UpdateHierarchy();
    }
}

Entity::Entity()
{

}


Entity::~Entity()
{
//    if(m_transform != nullptr)
//        delete m_transform;
//    if(m_model != nullptr)
//        delete m_model;
}

std::vector<std::shared_ptr<Entity>> &Entity::GetChildren()
{
    return m_children;
}

bool operator==(const Entity &lhs, const Entity &rhs)
{
    return lhs.getName() == rhs.getName() && lhs.getDepth() == rhs.getDepth();
}
