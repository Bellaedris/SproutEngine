#ifndef SPROUT_LIGHT_H
#define SPROUT_LIGHT_H

#include <glm/glm.hpp>

#include <sprout_engine/shader.h>
#include <sprout_engine/color.h>
#include <sprout_engine/inspectable.h>

#include <string>

using namespace glm;

class Light : public Inspectable
{
protected:
    Color color;
    float intensity{1};

public:
    Light() : color(1.f, 1.f, 1.f, 1.f) {};
    Light(Color col) : color(col) {};

    void set_colors(float* amb, float* diff, float* spec)
    {
        color = glm::vec4(amb[0], amb[1], amb[2], 1.f);
    };

    virtual void send_to_shader(Shader &s, int index) = 0;
    virtual void drawInspector(Camera *camera) = 0;
};

class DirectionalLight : public Light
{
protected:
    vec3 direction;
public:
    DirectionalLight() : Light(), direction(0., -1., 0.) {};
    DirectionalLight(glm::vec3 direction, Color color)
        : Light(color), direction(direction) {};

    static int idCount;

    [[nodiscard]] glm::vec3 getDirection() const
    {
        return glm::normalize(direction);
    };

    void set_direction(float* dir) { direction = vec3(dir[0], dir[1], dir[2]); };
    void set_direction(vec3 dir) { direction = dir; };

    void send_to_shader(Shader& s, int index) override
    {
        s.use();
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].direction"), direction[0], direction[1], direction[2]);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].intensity"), intensity);
        s.uniform_data(std::string("dirLights[").append(std::to_string(index)).append("].color"), color[0], color[1], color[2], color[3]);
    };

    void drawInspector(Camera *camera) override
    {
        if(ImGui::TreeNode("DirLight"))
        {
            ImGui::InputFloat("intensity", &intensity);
            ImGui::InputFloat4("direction", glm::value_ptr(direction));

            ImGui::ColorPicker4("ambiant", glm::value_ptr(color));

            ImGui::TreePop();
        }
    }
};

class PointLight : public Light
{
protected:
    vec3 position;
public:

    PointLight() : Light(), position(0., 0., 0.) {};
    PointLight(vec3 position, vec4 color)
     : Light(color),  position(position) {};

    void setPosition(glm::vec3 pos) {position = pos;};

    void send_to_shader(Shader& s, int index) override
    {
        s.use();
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].position"), position[0], position[1], position[2]);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].intensity"), intensity);
        s.uniform_data(std::string("pointLights[").append(std::to_string(index)).append("].color"), color[0], color[1], color[2], color[3]);
    };

    void drawInspector(Camera *camera) override
    {
        if(ImGui::TreeNode("PointLight"))
        {
            static ImGuizmo::OPERATION currentOp(ImGuizmo::TRANSLATE);

            ImGui::InputFloat3("Position", glm::value_ptr(position));

            glm::mat4 transform;
            auto scale = glm::one<vec3>();
            auto rot = glm::zero<vec3>();
            ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(position),
                                                    glm::value_ptr(scale),
                                                    glm::value_ptr(rot),
                                                    glm::value_ptr(transform));

            ImGuizmo::Manipulate(glm::value_ptr(camera->view()),
                                 glm::value_ptr(camera->projection()),
                                 currentOp,
                                 ImGuizmo::MODE::WORLD,
                                 glm::value_ptr(transform),
                                 nullptr,
                                 nullptr);

            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform),
                                                  glm::value_ptr(position),
                                                  glm::value_ptr(rot),
                                                  glm::value_ptr(scale)
            );

            ImGui::InputFloat("intensity", &intensity);
            ImGui::ColorPicker4("color", glm::value_ptr(color));
            ImGui::TreePop();
        }
    }
};

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