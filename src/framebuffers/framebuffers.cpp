#include <sprout_engine/SproutApp.h>

#include <sprout_engine/model.h>
#include <sprout_engine/texture.h>
#include <sprout_engine/light.h>
#include <sprout_engine/skybox.h>

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

std::string resources_path = "../../resources/";

class AppTest : public SproutApp
{
public:
    AppTest() : SproutApp(1366, 768, 4, 6) {}

    int init() override
    { 
        cam = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), 0., -90.);

       m = Model(resources_path + "models/bistro-small/export.obj", false);
       // m = Model(resources_path + "models/cube.obj", true);

        s = Shader("default.vs", "default.fs");
        s_post_process = Shader("postprocess.vs", "postprocess.fs");
        s_shadowmap = Shader("shadowmapping.vs", "shadowmapping.fs");
        s_skybox = Shader("skybox.vs", "skybox.fs");

        test_camera = Camera(glm::vec3(2.f, 0.f, -2.f), glm::vec3(0.f, 1.f, 0.f), 180.f, 0.f);

        light = DirectionalLight(glm::vec3(0.f, -1.f, 0.f), glm::vec3(.1, .1, .1), glm::vec3(1., 1., 1.), glm::vec3(.9, .9, .9));

        // post processing framebuffer/texture
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        fbo_texture = Texture(1366, 768, GL_RGB, GL_RGB);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture.get_id(), 0);

        fbo_depth = Texture(1366, 768, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo_depth.get_id(), 0);

        // shadow mapping framebuffer/texture
        glGenFramebuffers(1, &light_fb);
        glBindFramebuffer(GL_FRAMEBUFFER, light_fb);

        light_fb_depth = Texture(1366, 768, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, "texture_diffuse", GL_CLAMP_TO_BORDER);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light_fb_depth.get_id(), 0);

        //set screen buffer/vao
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &buffer);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        std::array<std::string, 6> cubemap = {
                resources_path + "textures/skyboxes/Daylight Box_Right.bmp",
                resources_path + "textures/skyboxes/Daylight Box_Left.bmp",
                resources_path + "textures/skyboxes/Daylight Box_Bottom.bmp",
                resources_path + "textures/skyboxes/Daylight Box_Top.bmp",
                resources_path + "textures/skyboxes/Daylight Box_Front.bmp",
                resources_path + "textures/skyboxes/Daylight Box_Back.bmp",
        };
        m_skybox = Skybox(cubemap);

        //init ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        glEnable(GL_DEPTH_TEST);

        return 0;
    }

    int render() override
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Parameters");
        if (ImGui::CollapsingHeader("performances"))
        {
            ImGui::PlotLines("framerate", framerate, IM_ARRAYSIZE(framerate), values_offset, "framerate", 0.f, 240, ImVec2(0, 80.f));
            int milli = (int)(gpu_last_frame / 1000000);
            int micro = (int)((gpu_last_frame / 1000) % 1000);
            ImGui::Text(
                "cpu %03dms\ngpu %02dms % 03dus",
                cpu_last_frame, 
                milli, micro
            );
        }
        if (ImGui::CollapsingHeader("Shadow mapping"))
        {
            ImGui::InputFloat2("Resolution", shadowmapRes);
            if (ImGui::Button("Update")) {
                glBindFramebuffer(GL_FRAMEBUFFER, light_fb);
                light_fb_depth = Texture(shadowmapRes[0], shadowmapRes[1], GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light_fb_depth.get_id(), 0);
            }
        }
        if (ImGui::CollapsingHeader("Parameters"))
        {
            ImGui::Checkbox("Wireframe", &wireframe_mode);
            ImGui::Checkbox("Show AABB", &show_aabb);
            ImGui::Checkbox("Show before post process", &noPostProcess);
            ImGui::InputFloat("FoV value", &fov, 1.f);
            ImGui::InputFloat("Aspect ratio w", &w, 1.f);
            ImGui::InputFloat("Aspect ratio h", &h, 1.f);
            ImGui::InputFloat3("Position of the object", objectPos);
            ImGui::InputFloat3("Direction of the light", lightDir);
            ImGui::ColorEdit3("light color ambiant", ambiant);
            ImGui::ColorEdit3("light color diffuse", diffuse);
            ImGui::ColorEdit3("light color specular", specular);
            if (ImGui::Button("update light")) {
                light.set_direction(lightDir);
                light.set_colors(ambiant, diffuse, specular);
            }
            if (ImGui::Button("reload shader")) {
                s = Shader("default.vs", "default.fs");
                s_post_process = Shader("postprocess.vs", "postprocess.fs");
                s_skybox = Shader("skybox.vs", "skybox.fs");
            }
            ImGui::InputFloat("rotation", &rotation, 1.f);
        }

        ImGui::End();

        //draw the scene from the light pov
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, light_fb);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, shadowmapRes[0], shadowmapRes[1]);
        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 light_model = glm::mat4(1.f);
        //light_model = glm::translate(light_model, sunPos);
        
        glm::mat4 light_view = glm::lookAt(glm::vec3(0., 30., 0.), glm::vec3(0., 0., 0.), glm::vec3(0., 0., 1.));

        glm::mat4 light_projection = glm::ortho(-50.f, 50.f, -50.f, 50.f, .1f, 100.f); // glm::perspective(glm::radians(fov), w / h, .1f, 100.f);

        glm::mat4 lightspaceMatrix = light_projection * light_view;

        s_shadowmap.use();
        s_shadowmap.uniform_data("mvpMatrix", lightspaceMatrix);

        m.draw(s_shadowmap);

        //then draw the scene in the camera pov
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glViewport(0, 0, 1366, 768);
        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = glfwGetTime();

        const float radius = 10.f;
        float camX = std::sin(glfwGetTime()) * radius;
        float camZ = std::cos(glfwGetTime()) * radius;

        glm::mat4 view = cam.view();
        
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
        s.uniform_data("lightspaceMatrix", lightspaceMatrix);

        s.uniform_data("shadowmap", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, light_fb_depth.get_id());
        m.draw(s, cam.get_frustum(w / h, fov, .1, 100), t);

        glDepthFunc(GL_LEQUAL);
        s_skybox.use();
        glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
        s_skybox.uniform_data("viewMatrix", skyboxView);
        s_skybox.uniform_data("projectionMatrix", projection);

        m_skybox.draw(s_skybox);

        if (noPostProcess)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glViewport(0, 0, 1366, 768);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            glBlitFramebuffer(0, 0, 1366, 768, 0, 0, 1366, 768, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
        else
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);

            s_post_process.use();
            glBindVertexArray(vao);
            glBindTexture(GL_TEXTURE_2D, fbo_texture.get_id());
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

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
    Model m;
    Shader s, s_post_process, s_shadowmap, s_skybox;
    Camera test_camera;
    DirectionalLight light;
    Skybox m_skybox;

    GLuint fbo;
    Texture fbo_texture;
    Texture fbo_depth;
    GLuint light_fb;
    Texture light_fb_depth;
    unsigned int vao;
    unsigned int buffer;

    glm::vec3 sunPos = glm::vec3(0., 10., 0.);

    // imgui inputs
    float color[4] = {0., 0., 1., 1. };
    float ambiant[4] = {.1, .1, .1, 1.};
    float diffuse[4] = {1., 1., 1., 1.};
    float specular[4] = { .9, .9, .9, 1. };
    float lightPos[3] = { 0.f, 10.f, 10.f};
    float lightDir[3] = { 0.f, 0.f, 0.f };
    float objectPos[3] = { 0., 0., 0. };
    float orbiter_light[3] = { 0.f, 0.f, 0.f };
    float shadowmapRes[2] = { 1366.f, 768.f };
    float w = 16.f;
    float h = 9.f;
    float rotation = 0.f;
    float last_frame_time_cpu;
    float last_frame_time_gpu;
    bool wireframe_mode = false;
    bool show_aabb = false;
    bool noPostProcess = false;

    float quadVertices[24] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    //framerate management
    float framerate[90] = {};
    int values_offset = 0;
};

int main()
{
    AppTest app;
    app.run();

    return 0;
}