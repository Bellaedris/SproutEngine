//
// Created by Bellaedris on 28/04/2024.
//

#ifndef SPROUTENGINE_ENTITY_H
#define SPROUTENGINE_ENTITY_H

#include "model.h"
#include "inspectable.h"

class Entity : public Inspectable {
protected:
    Transform m_transform;
public:
    const Transform &getTransform() const;

    const Model &getModel() const;

protected:
    Model m_model;

    std::string m_name{"entity"};

public:
    explicit Entity(const std::string& path, bool flip_uv = false);

    void draw(Shader &p_shader);
    void draw(Shader &p_shader, const Frustum& p_frustum, const Transform& p_transform);
    void drawInspector() override;
};


#endif //SPROUTENGINE_ENTITY_H
