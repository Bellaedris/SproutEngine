#ifndef SPROUT_LIGHT_H
#define SPROUT_LIGHT_H

#include <glm/glm.hpp>

#include<sprout_engine/shader.h>

#include <string>

using namespace glm;

class Light
{
protected:
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;

public:
    Light() : ambiant(vec3(.1f, .1f, .1f)), diffuse(vec3(1., 1., 1.)), specular(vec3(.9, .9, .9)) {};
    Light(vec3 amb, vec3 diff, vec3 spec) : ambiant(amb), diffuse(diff), specular(spec) {};

    void set_colors(float* amb, float* diff, float* spec)
    { 
        ambiant = glm::vec3(amb[0], amb[1], amb[2]); 
        diffuse = glm::vec3(diff[0], diff[1], diff[2]);
        specular = glm::vec3(spec[0], spec[1], spec[2]);
    };

    virtual void send_to_shader(Shader &s, int index) = 0;
};

class DirectionalLight : public Light
{
protected:
    vec3 direction;

public:
    DirectionalLight() : Light(), direction(vec3(0., -1., 0.)) {};
    DirectionalLight(glm::vec3 direction, glm::vec3 ambiant, glm::vec3 diffuse, glm::vec3 specular)
        : Light(ambiant, diffuse, specular), direction(direction) {};

    void set_direction(float* dir) { direction = vec3(dir[0], dir[1], dir[2]); };

    void send_to_shader(Shader& s, int index)
    {
        s.use();
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].direction"), direction[0], direction[1], direction[2]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].ambiant"), ambiant[0], ambiant[1], ambiant[2]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].diffuse"), diffuse[0], diffuse[1], diffuse[2]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].specular"), specular[0], specular[1], specular[2]);
    };
};

struct PointLight : public Light
{
protected:
    vec3 position;
    float constant;
    float linear;
    float quadratic;

public:
    PointLight() : Light(), position(vec3(0., 0., 0.)), constant(1.f), linear(.22f), quadratic(.2f) {};
    PointLight(vec3 position, vec3 ambiant, vec3 diffuse, vec3 specular, float constant = 1.f, float linear = .22f, float quadratic = .2f)
        : Light(ambiant, diffuse, specular),  position(position), constant(constant), linear(linear), quadratic(quadratic) {};
    
    void send_to_shader(Shader& s, int index)
    {

    };
};

struct SpotLight
{

};
#endif