//
// Created by Bellaedris on 28/04/2024.
//

#ifndef SPROUTENGINE_ENTITY_H
#define SPROUTENGINE_ENTITY_H

#include "model.h"
#include "inspectable.h"

class Entity : public Inspectable {
protected:
    Transform* m_transform {nullptr};
    Model* m_model {nullptr};
    std::vector<std::shared_ptr<Entity>> m_children {};
    Entity* m_parent {nullptr};
    int m_depth {0};

    std::string m_name{"entity"};
    bool m_bIsRootNode{true};

    void ForceUpdateHierarchy();

public:
    /***
     * \brief Empty constructor to build a root scene entity
     */
    explicit Entity();
    explicit Entity(const std::string& path, const std::string& name, bool flip_uv = false);
    ~Entity();

    void AddChild(const std::string& path, const std::string& name, bool flip_uv = false);
    void AddChild(std::shared_ptr<Entity> e);

    std::vector<std::shared_ptr<Entity>>& GetChildren();
    void SetPosition(const glm::vec3& position);
    void UpdateHierarchy();

    const Transform &getTransform() const;
    std::string getName() const { return m_name; }
    int getDepth() const { return m_depth; }

    void draw(Shader &p_shader);
    void draw(Shader &p_shader, const Frustum& p_frustum, const Transform& p_transform);
    void drawInspector(Camera *camera) override;
    void drawHierarchy(Entity &selectedEntity);
};

bool operator==(const Entity& lhs, const Entity& rhs);

#endif //SPROUTENGINE_ENTITY_H
