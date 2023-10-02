#include "sprout_engine/SproutApp.h"

#include "model.h"
#include "utils.h"
#include "texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <iostream>

class AppTest : public SproutApp
{
public:
    AppTest() : SproutApp(800, 600, 4, 6) {}

    int init()
    { 
        cam = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), 0., -90.);

        m = Model("data/models/backpack.obj");

        s = Shader("data/shaders/default_vertex_shader.glsl", "data/shaders/default_fragment_shader.glsl");

        t = Texture("data/textures/container.jpg", GL_CLAMP_TO_EDGE);
        t2 = Texture("data/textures/awesomeface.png");

        return 0;
    }

    int render()
    {

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Simple uniform value selector");
        ImGui::Checkbox("Wireframe", &wireframe_mode);
        ImGui::InputFloat("FoV value", &fov, 1.f);
        ImGui::InputFloat("Aspect ratio w", &w, 1.f);
        ImGui::InputFloat("Aspect ratio h", &h, 1.f);
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

        s.use();
        s.uniform_data("texture1", 0);
        s.uniform_data("texture2", 1);

        model = glm::mat4(1.f);

        mvpMatrix = projection * view * model;
        s.uniform_data("mvpMatrix", mvpMatrix);

        m.draw(s);

        if (wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    Model m;
    Shader s;
    Texture t, t2;

    // imgui inputs
    float color[4] = {0., 0., 1., 1. };
    float w = 4.f;
    float h = 3.f;
    bool wireframe_mode;
};

int main()
{
    AppTest app;
    app.run();

    return 0;
}