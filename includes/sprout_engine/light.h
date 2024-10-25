#ifndef SPROUT_LIGHT_H
#define SPROUT_LIGHT_H

#include <glm/glm.hpp>

#include<sprout_engine/shader.h>
#include<sprout_engine/color.h>

#include <string>

using namespace glm;

class Light //: public Inspectable
{
protected:
    Color ambiant;
    Color diffuse;
    Color specular;

public:
    Light() : ambiant(.1f, .1f, .1f, 1.f), diffuse(1.f, 1.f, 1.f, 1.f), specular(.9, .9, .9, 1.f) {};
    Light(Color amb, Color diff, Color spec) : ambiant(amb), diffuse(diff), specular(spec) {};

    void set_colors(float* amb, float* diff, float* spec)
    { 
        ambiant = glm::vec4(amb[0], amb[1], amb[2], 1.f);
        diffuse = glm::vec4(diff[0], diff[1], diff[2], 1.f);
        specular = glm::vec4(spec[0], spec[1], spec[2], 1.f);
    };

    virtual void send_to_shader(Shader &s, int index) = 0;
    //virtual void drawInspector() = 0;
};

class DirectionalLight : public Light
{
protected:
    vec4 direction;

public:
    DirectionalLight() : Light(), direction(0., -1., 0., 1.) {};
    DirectionalLight(glm::vec4 direction, Color ambiant, Color diffuse, Color specular)
        : Light(ambiant, diffuse, specular), direction(direction) {};

    [[nodiscard]] glm::vec4 getDirection() const
    {
        return direction;
    };

    void set_direction(float* dir) { direction = vec4(dir[0], dir[1], dir[2], 1.f); };

    void send_to_shader(Shader& s, int index) override
    {
        s.use();
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].direction"), direction[0], direction[1], direction[2], direction[3]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].ambiant"), ambiant[0], ambiant[1], ambiant[2], ambiant[3]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].diffuse"), diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].specular"), specular[0], specular[1], specular[2], specular[3]);
    };

    // void drawInspector() override
    // {
    //
    // }
};

class PointLight : public Light
{
protected:
    vec4 position;
    float constant;
    float linear;
    float quadratic;

public:
    PointLight() : Light(), position(0., 0., 0., 1.), constant(1.f), linear(.22f), quadratic(.2f) {};
    PointLight(vec4 position, vec4 ambiant, vec4 diffuse, vec4 specular, float constant = 1.f, float linear = .22f, float quadratic = .2f)
        : Light(ambiant, diffuse, specular),  position(position), constant(constant), linear(linear), quadratic(quadratic) {};
    
    void send_to_shader(Shader& s, int index) override
    {
        s.use();
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].position"), position[0], position[1], position[2], position[3]);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].ambiant"), ambiant[0], ambiant[1], ambiant[2], ambiant[3]);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].diffuse"), diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].specular"), specular[0], specular[1], specular[2], specular[3]);

        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].constant"), constant);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].linear"), linear);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].quadratic"), quadratic);
    };
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutoff;


};
#endif