//
// Created by Bellaedris on 28/04/2024.
//

#ifndef SPROUTENGINE_ENTITY_H
#define SPROUTENGINE_ENTITY_H

#include "model.h"
#include "inspectable.h"

class Entity : public Inspectable {
public:
    const Transform &getTransform() const;

    const Model &getModel() const;

protected:
    Transform m_transform;
    Model m_model;

    std::string m_name{"entity"};

public:
    explicit Entity(const std::string& path, const std::string& name = "entity", bool flip_uv = false);

    void UpdatePosition(const glm::vec3& position);

    void draw(Shader &p_shader);
    void draw(Shader &p_shader, const Frustum& p_frustum, const Transform& p_transform);
    void drawInspector() override;
};


#endif //SPROUTENGINE_ENTITY_H
