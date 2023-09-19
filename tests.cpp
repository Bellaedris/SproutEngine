#include "sprout_engine/SproutApp.h"

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

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // vertex buffer
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        // index buffer
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

        //vao attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        //glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);

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

        t.use();
        t2.use(GL_TEXTURE1);
        glBindVertexArray(vao);
        //glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        for (int i = 0; i < cubePositions.size(); i++)
        {
            model = glm::mat4(1.f);
            model = glm::translate(model, cubePositions[i]);
            model = i % 3 == 0 ? glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f)) : glm::rotate(model, glm::radians(20.f * i), glm::vec3(1.0f, 0.3f, 0.5f));

            mvpMatrix = projection * view * model;
            s.uniform_data("mvpMatrix", mvpMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        

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
    unsigned int buffer;
    unsigned int index_buffer;
    unsigned int vao;
    float vertices[180] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    std::vector<int> indices = {
        0, 1, 3,
        1, 2, 3
    };
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