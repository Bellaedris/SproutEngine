#include "sprout_engine/SproutApp.h"

#include "model.h"
#include "utils.h"
#include "texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <irrklang/irrKlang.h>

#include <vector>
#include <iostream>
#include <random>
#include <ctime>

int pick_random_index(int previous, int size)
{
    srand(std::time(nullptr));

    int index = rand() % size;
    while (index == previous)
        index = rand() % size;

    return index;
}

class AppTest : public SproutApp
{
public:
    AppTest() : SproutApp(1366, 768, 4, 6) {}

    int init()
    { 
        cam = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), 0., -90.);

        //m = Model("data/models/bistro/exterior.obj", true);
        m = Model("data/models/backpack.obj", true);

        m2 = Model("data/models/cube.obj");
        spinners.push_back(Model("data/models/Cat/12221_Cat_v1_l3.obj"));
        spinners.push_back(Model("data/models/Duck/12248_Bird_v1_L2.obj"));
        spinners.push_back(Model("data/models/Frog/12268_banjofrog_v1_L3.obj"));
        spinners.push_back(Model("data/models/Horse/10026_Horse_v01-it2.obj"));

        s = Shader("data/shaders/default_vertex_shader.glsl", "data/shaders/default_fragment_shader.glsl");
        s_debug_aabb = Shader("data/shaders/debug_lines_vertex.glsl", "data/shaders/debug_lines_fragment.glsl");

        SoundEngine = irrklang::createIrrKlangDevice();

        return 0;
    }

    int render()
    {
        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Parameters");
        ImGui::Checkbox("Wireframe", &wireframe_mode);
        ImGui::InputFloat("FoV value", &fov, 1.f);
        ImGui::InputFloat("Aspect ratio w", &w, 1.f);
        ImGui::InputFloat("Aspect ratio h", &h, 1.f);
        ImGui::InputFloat3("Position of the light", lightPos);
        ImGui::Checkbox("Use moving light source", &use_orbiter_light);
        ImGui::ColorEdit3("color ambiant", ambiant);
        ImGui::ColorEdit3("color diffuse", diffuse);
        ImGui::ColorEdit3("color specular", specular);
        if (ImGui::Button("Spin an animal")) {
            spin = true;
            music_index = (music_index + 1) % 2;
            SoundEngine->stopAllSounds();
            SoundEngine->play2D(musics[music_index].c_str(), true, true);
            animal_to_draw = pick_random_index(animal_to_draw, spinners.size());
            SoundEngine->setAllSoundsPaused(false);
        }
        ImGui::Checkbox("Spin model", &spin);

        ImGui::PlotLines("framerate", framerate, IM_ARRAYSIZE(framerate), values_offset, "framerate", 0.f, 240, ImVec2(0, 80.f));

        ImGui::End();

        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = glfwGetTime();

        glm::mat4 model;// = glm::mat4(1.f);
        // model = glm::rotate(model, (float) glfwGetTime() * glm::radians(55.f), glm::vec3(.5f, 1.f, 0.f));

        const float radius = 10.f;
        float camX = std::sin(glfwGetTime()) * radius;
        float camZ = std::cos(glfwGetTime()) * radius;

        glm::mat4 view = cam.view();
        view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

        glm::mat4 projection = glm::perspective(glm::radians(fov), w / h, .1f, 100.f);

        glm::mat4 mvpMatrix; // = projection * view * model;
        glm::mat4 inverseViewMatrix, normalMatrix;

        s.uniform_data("light.position", lightPos[0], lightPos[1], lightPos[2]);
        s.uniform_data("light.ambiant", ambiant[0], ambiant[1], ambiant[2]);
        s.uniform_data("light.diffuse", diffuse[0], diffuse[1], diffuse[2]);
        s.uniform_data("light.specular", specular[0], specular[1], specular[2]);

        model = glm::mat4(1.f);
        if (spin)
            model = glm::rotate(model, (float)time, glm::vec3(0., 1., 0.));
        model = glm::scale(model, glm::vec3(.1, .1, .1));

        normalMatrix = glm::transpose(glm::inverse(model));
        mvpMatrix = projection * view * model;
        inverseViewMatrix = glm::inverse(view);
        s.uniform_data("modelMatrix", model);
        s.uniform_data("normalMatrix", normalMatrix);
        s.uniform_data("mvpMatrix", mvpMatrix);
        s.uniform_data("inverseViewMatrix", inverseViewMatrix);

        s_debug_aabb.uniform_data("mvpMatrix", glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f)));

        if (animal_to_draw == -1) {
            m.draw(s);
            m.drawAABB(s_debug_aabb);
        }
        else
            spinners[animal_to_draw].draw(s);

        // second cube to visualize light
        if (use_orbiter_light) {
            lightPos[0] = std::cos(time) * 3.;
            lightPos[2] = std::sin(time) * 3.;
        }
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
        s.uniform_data("light_source", lightPos[0], lightPos[1], lightPos[2]);
        mvpMatrix = projection * view * model;
        inverseViewMatrix = glm::inverse(view);
        s.uniform_data("modelMatrix", model);
        s.uniform_data("mvpMatrix", mvpMatrix);
        s.uniform_data("inverseViewMatrix", inverseViewMatrix);

        m2.draw(s);

        if (wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // update framerate
        framerate[values_offset] = 1.f / delta_time;
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(framerate);

        return 0;
    }

    int quit()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        return 0;
    }

protected:
    std::vector<Model> spinners;
    Model m;
    Model m2;
    Shader s;
    Shader s_debug_aabb;

    // imgui inputs
    float color[4] = {0., 0., 1., 1. };
    float ambiant[4] = {.1, .1, .1, 1.};
    float diffuse[4] = {1., 1., 1., 1.};
    float specular[4] = { .9, .9, .9, 1. };
    float lightPos[3] = { 0.f, 10.f, 10.f};
    float orbiter_light[3] = { 0.f, 0.f, 0.f };
    float w = 16.f;
    float h = 9.f;
    int animal_to_draw = -1;
    int music_index;
    bool wireframe_mode;
    bool use_orbiter_light;
    bool spin = false;

    //framerate management
    float framerate[90] = {};
    int values_offset = 0;

    std::string musics[2] =
    {
        "data/audio/free bird guitar solo but its low quality.mp3",
        "data/audio/funky town low quality.mp3"
    };

    irrklang::ISoundEngine* SoundEngine;
};

int main()
{
    AppTest app;
    app.run();

    return 0;
}