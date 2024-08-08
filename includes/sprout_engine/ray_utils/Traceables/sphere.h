//
// Created by Bellaedris on 04/08/2024.
//

#ifndef SPROUTENGINE_SPHERE_H
#define SPROUTENGINE_SPHERE_H


#include "traceable.h"

class Sphere : public Traceable
{
public:
    Sphere(const glm::vec3& p_center, float p_radius);
    bool hit(const Ray &r, Interval<float> p_t, HitInfo &hitInfo) const override;

private:
    glm::vec3 m_center;
    float m_radius;
};


#endif //SPROUTENGINE_SPHERE_H
