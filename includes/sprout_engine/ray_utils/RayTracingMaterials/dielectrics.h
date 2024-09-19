//
// Created by arpradier on 19/09/2024.
//


#pragma once

#include "raytracingMaterial.h"

class Dielectric : public RaytracingMaterial {
public:
    explicit Dielectric(float p_refractiveIndex) : m_refractiveIndex(p_refractiveIndex) {}
    bool scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const override;

private:
    /**
     * \brief The reflectance of glass varies with the angle we're looking at it. We use Schlick's approximation
     * to compute that.
     * @param p_cosine Cosine between the ray and the surface normal
     * @return reflectance index
     */
    double reflectance(float p_cosine) const;

    // index of refraction: refraction of the material of the sphere divided by the material of the world
    float m_refractiveIndex;
};
