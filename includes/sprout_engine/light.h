#ifndef SPROUT_LIGHT_H
#define SPROUT_LIGHT_H

#include <glm/glm.hpp>

#include <sprout_engine/shader.h>
#include <sprout_engine/color.h>
#include <sprout_engine/inspectable.h>

#include <string>

#include <IMGUI/imgui.h>

using namespace glm;

class Light : public Inspectable
{
public:
    [[nodiscard]] Color GetAmbiant() const
    {
        return ambiant;
    }

    [[nodiscard]] Color GetDiffuse() const
    {
        return diffuse;
    }

    [[nodiscard]] Color GetSpecular() const
    {
        return specular;
    }

protected:
    Color ambiant;
    Color diffuse;
    Color specular;
    float intensity{1};

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
    virtual void drawInspector() = 0;
};

class DirectionalLight : public Light
{
protected:
    vec4 direction;
public:
    static int numberOfLights;

    DirectionalLight() : Light(), direction(0., -1., 0., 1.) {};
    DirectionalLight(glm::vec4 direction, Color ambiant, Color diffuse, Color specular)
        : Light(ambiant, diffuse, specular), direction(direction) {};

    [[nodiscard]] glm::vec4 getDirection() const
    {
        return direction;
    };

    void set_direction(float* dir) { direction = vec4(dir[0], dir[1], dir[2], 1.f); };
    void set_direction(vec3 dir) { direction = vec4(dir, 1.f); };

    void send_to_shader(Shader& s, int index) override
    {
        s.use();
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].direction"), direction[0], direction[1], direction[2], direction[3]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].ambiant"), ambiant[0], ambiant[1], ambiant[2], ambiant[3]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].diffuse"), diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].specular"), specular[0], specular[1], specular[2], specular[3]);
    };

    void drawInspector() override
    {
        if(ImGui::TreeNode(std::string("DirLight" + std::to_string(numberOfLights)).c_str()))
        {
            ImGui::InputFloat4("direction", glm::value_ptr(direction));

            ImGui::ColorPicker4("ambiant", glm::value_ptr(ambiant));
            ImGui::ColorPicker4("diffuse", glm::value_ptr(diffuse));
            ImGui::ColorPicker4("specular", glm::value_ptr(specular));

            ImGui::TreePop();
        }
    }
};

int DirectionalLight::numberOfLights = 0;

class PointLight : public Light
{
protected:
    vec4 position;
    float constant;
    float linear;
    float quadratic;

public:
    static int numberOfLights;

    PointLight() : Light(), position(0., 0., 0., 1.), constant(1.f), linear(.22f), quadratic(.2f) {};
    PointLight(vec4 position, vec4 ambiant, vec4 diffuse, vec4 specular, float constant = 1.f, float linear = .22f, float quadratic = .2f)
     : Light(ambiant, diffuse, specular),  position(position), constant(constant), linear(linear), quadratic(quadratic) {};

    void send_to_shader(Shader& s, int index) override
    {
        s.use();
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].position"), position[0], position[1], position[2], position[3]);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].ambiant"), ambiant[0], ambiant[1], ambiant[2], ambiant[3]);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].diffuse"), diffuse[0] * intensity, diffuse[1] * intensity, diffuse[2] * intensity, diffuse[3]);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].specular"), specular[0] * intensity, specular[1] * intensity, specular[2] * intensity, specular[3]);

        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].constant"), constant);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].linear"), linear);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].quadratic"), quadratic);
    };

    void drawInspector() override
    {
        if(ImGui::TreeNode(std::string("PointLight" + std::to_string(numberOfLights)).c_str()))
        {
            ImGui::InputFloat4("position", glm::value_ptr(position));
            ImGui::InputFloat("intensity", &intensity);
            ImGui::InputFloat("constant", &constant);
            ImGui::InputFloat("linear", &linear);
            ImGui::InputFloat("quadratic", &quadratic);

            ImGui::ColorPicker4("ambiant", glm::value_ptr(ambiant));
            ImGui::ColorPicker4("diffuse", glm::value_ptr(diffuse));
            ImGui::ColorPicker4("specular", glm::value_ptr(specular));

            ImGui::TreePop();
        }
    }
};

int PointLight::numberOfLights = 0;

//
// struct SpotLight
// {
//     vec3 position;
//     vec3 direction;
//     float cutoff;
//
//
// };
#endif