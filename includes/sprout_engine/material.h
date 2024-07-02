//
// Created by Bellaedris on 16/06/2024.
//

#ifndef SPROUTENGINE_MATERIAL_H
#define SPROUTENGINE_MATERIAL_H

#include "color.h"


struct Material {
    Color diffuse{};
    Color specular{};
    Color emissive{};
    int diffuse_texture{-1};
    int specular_texture{-1};
    int emission_texture{-1};

    Material() : diffuse(), specular(), emissive(), diffuse_texture(-1), specular_texture(-1), emission_texture(-1) {}
    Material(const Color& color) : diffuse(color), specular(), emissive(), diffuse_texture(-1), specular_texture(-1), emission_texture(-1) {}
};


#endif //SPROUTENGINE_MATERIAL_H
