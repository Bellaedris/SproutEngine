#include <sprout_engine/SproutApp.h>

#include <sprout_engine/texture.h>
#include <sprout_engine/light.h>
#include <sprout_engine/skybox.h>
#include <sprout_engine/entity.h>

#include <glm/glm.hpp>

#include <vector>
#include "sprout_engine/passes/ColorPass.h"
#include "sprout_engine/passes/tonemappingPass.h"
#include "sprout_engine/buffer.h"

std::string resources_path = "../../resources/";

class AppPBR : public SproutApp
{
public:
    AppPBR() : SproutApp(1366, 768, 4, 6) {}

    int init() override
    {
        playerCamera = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), 0., -90., 0.1f, 1000.f, fov, 16.f / 9.f);
        setActiveCamera(&playerCamera);

        m_entities.emplace_back(resources_path + "models/DamagedHelmet/DamagedHelmet.gltf", "helmet", false);

        s = Shader("PBR.vs", "PBR.fs");
        s_post_process = Shader("postprocess.vs", "postprocess.fs");
        s_skybox = Shader("skybox.vs", "skybox.fs");
        s_tonemapping = Shader("tonemapping.vs", "tonemapping.fs");

        m_colorPass = std::make_unique<ColorPass>(m_width, m_height);
        m_tonemappingPass = std::make_unique<TonemappingPass>(m_width, m_height);
        m_tonemappingPass->m_bIsFinal = true;

        m_pointLights =
        {
                {
                        {1., 1., 1., 1.},
                        {1., 1., 1., 1.},
                        {0., 1., 0., 1.},
                        {1., 1., 1., 1.}
                }
        };

        m_dirLights =
        {
            {
                    {0., 1., 0., 1.},
                    {1., 1., 1., 1.},
                    {1., 1., 1., 1.},
                    {1., 1., 1., 1.}
            }
        };

         std::array<std::string, 6> cubemap = {
                 resources_path + "textures/skyboxes/right.bmp",
                 resources_path + "textures/skyboxes/left.bmp",
                 resources_path + "textures/skyboxes/bottom.bmp",
                 resources_path + "textures/skyboxes/top.bmp",
                 resources_path + "textures/skyboxes/front.bmp",
                 resources_path + "textures/skyboxes/back.bmp",
         };
         m_skybox = Skybox(cubemap);

        glEnable(GL_DEPTH_TEST);

        return 0;
    }

    int render() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // IMGUI
        {
            // inspector
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Inspector");
            // mainCamera inspector
            playerCamera.drawInspector();
            for (auto &entity: m_entities)
                entity.drawInspector();
            int i = 0;
            for(auto& pointLight : m_pointLights)
            {
                PointLight::numberOfLights = i;
                pointLight.drawInspector();
                i++;
            }
            i = 0;
            for(auto& dirLight : m_dirLights)
            {
                DirectionalLight::numberOfLights = i;
                dirLight.drawInspector();
                i++;
            }
            ImGui::End();

            // general settings
            ImGui::Begin("Parameters");
            if (ImGui::CollapsingHeader("performances")) {
                ImGui::PlotLines("framerate", framerate, IM_ARRAYSIZE(framerate), values_offset, "framerate", 0.f, 240,
                                 ImVec2(0, 80.f));
                int milli = (int) (gpu_last_frame / 1000000);
                int micro = (int) ((gpu_last_frame / 1000) % 1000);
                ImGui::Text(
                        "cpu %03dms\ngpu %02dms % 03dus",
                        cpu_last_frame,
                        milli, micro
                );
            }
            if (ImGui::CollapsingHeader("General Parameters")) {
                ImGui::Checkbox("Wireframe", &wireframe_mode);
                ImGui::Checkbox("Show before post process", &noPostProcess);
                ImGui::Checkbox("Draw skybox", &drawSkybox);
                ImGui::InputFloat("Gamma correction", &gamma, 0.1f);
                ImGui::InputFloat("Exposure", &exposure, .1f);
                if (ImGui::Button("reload shader")) {
                    s = Shader("PBR.vs", "PBR.fs");
                    s_post_process = Shader("postprocess.vs", "postprocess.fs");
                    s_skybox = Shader("skybox.vs", "skybox.fs");
                    s_tonemapping = Shader("tonemapping.vs", "tonemapping.fs");
                }
            }

            ImGui::End();
        }

        m_pointLights[0].setPosition(glm::vec4(std::cosf(timeSinceStartup) * 10.f, 0.f, std::sinf(timeSinceStartup) * 10.f, 1.f));

        //draw the scene
        s.use();
        s.uniform_data("gamma", gamma);
        s.uniform_data("shadowmap", 2);
        s.uniform_data("pointLightsNumber", (int)m_pointLights.size());
        for(int i = 0; i < m_pointLights.size(); i++)
            m_pointLights[i].send_to_shader(s, i);

        s.uniform_data("dirLightsNumber", (int)m_dirLights.size());
        for(int i = 0; i < m_dirLights.size(); i++)
            m_dirLights[i].send_to_shader(s, i);

        m_colorPass->render(m_entities, *mainCamera, s);

        if (drawSkybox) {
            glDepthFunc(GL_LEQUAL);
            s_skybox.use();
            glm::mat4 skyboxView = glm::mat4(glm::mat3(mainCamera->view()));
            s_skybox.uniform_data("viewMatrix", skyboxView);
            s_skybox.uniform_data("projectionMatrix", mainCamera->projection());

            m_skybox.draw(s_skybox);
        }

        // apply tonemapping
        s_tonemapping.use();
        s_tonemapping.uniform_data("exposure", exposure);
        m_colorPass->activateTexture();
        s_tonemapping.uniform_data("frame", 0);
        m_tonemappingPass->render(m_entities, *mainCamera, s_tonemapping);


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
    std::vector<Entity> m_entities;
    Shader s, s_post_process, s_skybox, s_tonemapping;
    Camera playerCamera;
    Skybox m_skybox;

    GLuint fbo;
    Texture fbo_texture;
    Texture fbo_depth;
    unsigned int quad_vao;
    unsigned int quad_buffer;
    std::vector<PointLight> m_pointLights;
    std::vector<DirectionalLight> m_dirLights;

    std::unique_ptr<ColorPass> m_colorPass;
    std::unique_ptr<TonemappingPass> m_tonemappingPass;

    // imgui inputs
    float gamma = 2.2f;
    float exposure = 1.f;
    bool wireframe_mode = false;
    bool noPostProcess = false;
    bool drawSkybox = true;

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
    AppPBR app;
    app.run();

    return 0;
}