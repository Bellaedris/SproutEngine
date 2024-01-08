#include <sprout_engine/SproutApp.h>

#include <sprout_engine/model.h>
#include <sprout_engine/texture.h>
#include <sprout_engine/light.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <irrklang/irrKlang.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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

std::string resources_path = "../../../resources/";

class AppTest : public SproutApp
{
public:
    AppTest() : SproutApp(1366, 768, 4, 6) {}

    int init()
    { 
        cam = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), 0., -90.);

        m = Model(resources_path + "models/bistro-small/export.obj", true);
        //m = Model(resources_path + "models/backpack.obj", true);

        m2 = Model(resources_path + "models/cube.obj");
        spinners.push_back(Model(resources_path + "models/Cat/12221_Cat_v1_l3.obj"));
        spinners.push_back(Model(resources_path + "models/Duck/12248_Bird_v1_L2.obj"));
        spinners.push_back(Model(resources_path + "models/Frog/12268_banjofrog_v1_L3.obj"));
        spinners.push_back(Model(resources_path + "models/Horse/10026_Horse_v01-it2.obj"));

        s = Shader("default.vs", "default.fs");
        s_debug_aabb = Shader("debug_lines.vs", "debug_lines.fs");
        s_debug_frustum = Shader("debug_lines.vs", "plain_color.fs");

        test_camera = Camera(glm::vec3(2.f, 0.f, -2.f), glm::vec3(0.f, 1.f, 0.f), 180.f, 0.f);

        light = DirectionalLight(glm::vec3(0.f, -1.f, 0.f), glm::vec3(.1, .1, .1), glm::vec3(1., 1., 1.), glm::vec3(.9, .9, .9));

        SoundEngine = irrklang::createIrrKlangDevice();

        //init ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        return 0;
    }

    int render()
    {
        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Parameters");
        ImGui::PlotLines("framerate", framerate, IM_ARRAYSIZE(framerate), values_offset, "framerate", 0.f, 240, ImVec2(0, 80.f));
        ImGui::Checkbox("Wireframe", &wireframe_mode);
        ImGui::Checkbox("Show AABB", &show_aabb);
        ImGui::InputFloat("FoV value", &fov, 1.f);
        ImGui::InputFloat("Aspect ratio w", &w, 1.f);
        ImGui::InputFloat("Aspect ratio h", &h, 1.f);
        ImGui::InputFloat3("Direction of the light", lightDir);
        ImGui::InputFloat3("Position of the object", objectPos);
        ImGui::Checkbox("Use moving light source", &use_orbiter_light);
        ImGui::ColorEdit3("light color ambiant", ambiant);
        ImGui::ColorEdit3("light color diffuse", diffuse);
        ImGui::ColorEdit3("light color specular", specular);
        if (ImGui::Button("update light")) {
            light.set_direction(lightDir);
            light.set_colors(ambiant, diffuse, specular);
        }
        ImGui::InputFloat("rotation", &rotation, 1.f);
        if (ImGui::Button("Spin an animal")) {
            spin = true;
            music_index = (music_index + 1) % 2;
            SoundEngine->stopAllSounds();
            SoundEngine->play2D(musics[music_index].c_str(), true, true);
            animal_to_draw = pick_random_index(animal_to_draw, spinners.size());
            SoundEngine->setAllSoundsPaused(false);
        }
        ImGui::Checkbox("Spin model", &spin);

        ImGui::End();

        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = glfwGetTime();

        const float radius = 10.f;
        float camX = std::sin(glfwGetTime()) * radius;
        float camZ = std::cos(glfwGetTime()) * radius;

        glm::mat4 view = cam.view();
        view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

        glm::mat4 projection = glm::perspective(glm::radians(fov), w / h, .1f, 100.f);

        glm::mat4 mvpMatrix; // = projection * view * model;
        glm::mat4 inverseViewMatrix, normalMatrix;

        s.use();
        light.send_to_shader(s, 0);

        Transform t;
        t.setLocalScale(glm::vec3(.1, .1, .1));
        t.setLocalPosition(glm::vec3(objectPos[0], objectPos[1], objectPos[2]));
        t.computeModelMatrix();

        glm::mat4 model = glm::mat4(1.f);
        if (spin)
            model = glm::rotate(model, (float)time, glm::vec3(0., 1., 0.));
        model = glm::translate(model, glm::vec3(objectPos[0], objectPos[1], objectPos[2]));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0., 0., 1.));
        
        //model = glm::scale(model, glm::vec3(.1, .1, .1));

        normalMatrix = glm::transpose(glm::inverse(model));
        mvpMatrix = projection * view * model;
        inverseViewMatrix = glm::inverse(view);
        s.uniform_data("modelMatrix", model);
        s.uniform_data("normalMatrix", normalMatrix);
        s.uniform_data("mvpMatrix", mvpMatrix);
        s.uniform_data("inverseViewMatrix", inverseViewMatrix);

        glm::mat4 mvpFrustum = projection * view;

        glm::mat3 rotation(model);
        glm::vec3 translation(model[3]);

        if (animal_to_draw == -1) {
            if (show_aabb) {
                s_debug_aabb.use();
                s_debug_aabb.uniform_data("mvpMatrix", projection*view);
                m.drawAABB(s_debug_aabb, rotation, translation);
            }
            m.draw(s, cam.get_frustum(w / h, fov, .1, 100), t);
            /*s_debug_frustum.use();
            s_debug_frustum.uniform_data("mvpMatrix", mvpMatrix);
            m.draw_debug_frustum(s_debug_frustum, test_camera.get_frustum(w / h, fov, .1, 100), t);
            float aspect = h / w;
            s_debug_aabb.use();
            s_debug_aabb.uniform_data("mvpMatrix", mvpFrustum);
            test_camera.draw_frustum(s_debug_aabb, aspect, fov, .1, 100);*/
        }
        else
            spinners[animal_to_draw].draw(s, cam.get_frustum(w / h, fov, .1, 100), t);

        // second cube to visualize light
        s.use();
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

        m2.draw(s, cam.get_frustum(w / h, fov, 1, 100), t);

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
    Shader s, s_debug_aabb, s_debug_frustum;
    Camera test_camera;
    DirectionalLight light;

    // imgui inputs
    float color[4] = {0., 0., 1., 1. };
    float ambiant[4] = {.1, .1, .1, 1.};
    float diffuse[4] = {1., 1., 1., 1.};
    float specular[4] = { .9, .9, .9, 1. };
    float lightPos[3] = { 0.f, 10.f, 10.f};
    float lightDir[3] = { 0.f, 0.f, 0.f };
    float objectPos[3] = { 0., 0., 0. };
    float orbiter_light[3] = { 0.f, 0.f, 0.f };
    float w = 16.f;
    float h = 9.f;
    float rotation = 0.f;
    int animal_to_draw = -1;
    int music_index;
    bool wireframe_mode;
    bool show_aabb = false;
    bool use_orbiter_light;
    bool spin = false;

    //framerate management
    float framerate[90] = {};
    int values_offset = 0;

    std::string musics[2] =
    {
        "resources/audio/free bird guitar solo but its low quality.mp3",
        "resources/audio/funky town low quality.mp3"
    };

    irrklang::ISoundEngine* SoundEngine;
};

int main()
{
    AppTest app;
    app.run();

    return 0;
}