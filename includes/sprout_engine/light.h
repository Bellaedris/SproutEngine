#ifndef SPROUT_LIGHT_H
#define SPROUT_LIGHT_H

#include <glm/glm.hpp>

struct DirectionalLight
{
    glm::vec3 direction;

    glm::vec3 ambiant;
    glm::vec3 diffuse;
    glm::vec3 specular;

    DirectionalLight() = default;
    DirectionalLight(glm::vec3 direction, glm::vec3 ambiant, glm::vec3 diffuse, glm::vec3 specular)
        : direction(direction),ambiant(ambiant), diffuse(diffuse), specular(specular) {};
};

struct PointLight
{
    
};

struct SpotLight
{

};
#endif