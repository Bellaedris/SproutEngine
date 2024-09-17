//
// Created by arpradier on 17/09/2024.
//

#pragma once

#include "raytracingMaterial.h"

class Lambertian : public RaytracingMaterial
{
public:
    explicit Lambertian(const Color& albedo) : albedo(albedo) {}

    bool scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const override;

private:
    Color albedo;
};
