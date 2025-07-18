//
// Created by Bellaedris on 26/01/2025.
//

#include <sprout_engine/SproutApp.h>

#include <sprout_engine/light.h>
#include <sprout_engine/entity.h>
#include <sprout_engine/cubemapHDRI.h>

#include <vector>
#include "sprout_engine/passes/forwardPass.h"
#include "sprout_engine/passes/tonemappingPass.h"
#include "sprout_engine/buffer.h"
#include "sprout_engine/passes/ChromaticAberrationPass.h"
#include "sprout_engine/passes/filmGrainPass.h"
#include "IMGUI/ImGuiFileDialog.h"
#include "sprout_engine/passes/shadowmappingPass.h"
#include "sprout_engine/passes/bloomPass.h"

std::string resources_path = "../../resources/";

// uv settings to write in the ImGui viewport using OpenGL coord. system
const ImVec2 uv_min = ImVec2(0.0f, 1.0f);
const ImVec2 uv_max = ImVec2(1.0f, 0.0f);

class SproutEngineEditor : public SproutApp
{
public:
    SproutEngineEditor() : SproutApp(1360, 768, 4, 6) {}

    void loadAsset()
    {
        IGFD::FileDialogConfig config;
        config.path = "../../resources";
        ImGuiFileDialog::Instance()->OpenDialog(
                "ChooseFileDlgKey",
                "Open 3D File",
                ".gltf,.glb,.obj,.fbx",
                config
        );

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                // action
                m_sceneRoot->AddChild(filePathName, "");
                pickingFile = false;
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
    }

    void addLight(int type)
    {
        switch (type)
        {
            case 1:
                m_dirLights.emplace_back();
                break;
            case 2:
                m_pointLights.emplace_back();
                break;
        }
    }

    int init() override
    {
        playerCamera = Camera(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), 0., -90., 0.1f, 1000.f, fov, 16.f / 9.f);
        setActiveCamera(&playerCamera);

        //m_entities.emplace_back(resources_path + "models/Sponza/glTF/Sponza.gltf", "", false);

        s = Shader("PBR.vs", "PBR.fs");
        s_skybox = Shader("skybox.vs", "skybox.fs");
        s_shadowmapping = Shader("shadowmapping.vs", "shadowmapping.fs");

        m_shadowPass = std::make_unique<ShadowmappingPass>(shadowmapResolution, shadowmapResolution);
        m_colorPass = std::make_unique<ForwardPass>(m_width, m_height);
        m_PPtechniques.emplace_back(std::make_unique<BloomPass>(m_width, m_height, "postprocess.vs", "bloom.fs", "postprocess.vs", "brightPass.fs", "gaussianBlur.fs"));
        m_PPtechniques.emplace_back(std::make_unique<TonemappingPass>(m_width, m_height, "postprocess.vs", "tonemapping.fs"));
        m_PPtechniques.emplace_back(std::make_unique<ChromaticAberrationPass>(m_width, m_height, "postprocess.vs", "chromaticAberration.fs"));
        m_PPtechniques.emplace_back(std::make_unique<FilmGrainPass>(m_width, m_height, "postprocess.vs", "filmGrain.fs"));

        m_pointLights =
                {
                        {
                                {1., 1., 1.},
                                {1., 1., 1., 1.},
                        }
                };

        m_dirLights =
                {
                        {
                                {0., -1., 3.},
                                {1., 1., 1., 1.},
                        }
                };

        m_skybox = HDRCubemap(std::string(resources_path + "textures/skyboxes/lilienstein_4k.hdr").c_str());

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        glEnable(GL_DEPTH_TEST);

        return 0;
    }

    int render() override {
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, m_width, m_height);

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        // menu at the top of the window
        if(ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Load model", NULL))
                    loadAsset();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Assets"))
            {
                ImGui::MenuItem("Load model", nullptr, &pickingFile);
                if(ImGui::BeginMenu("Lights"))
                {
                    if (ImGui::MenuItem("Directionnal light", NULL))
                        addLight(1);
                    if (ImGui::MenuItem("Point light", NULL))
                        addLight(2);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        //m_pointLights[0].setPosition(glm::vec4(std::cosf(timeSinceStartup) * 10.f, 0.f, std::sinf(timeSinceStartup) * 10.f, 1.f));

        //depth pre-pass
        m_shadowPass->render(m_sceneRoot, m_dirLights[0], s_shadowmapping);

        //draw the scene
        s.use();
        // IBL textures
        s.uniform_data("irradianceMap", 5);
        m_skybox.useIrradiance(5);
        s.uniform_data("prefilterMap", 6);
        m_skybox.usePrefilter(6);
        s.uniform_data("brdfLUT", 7);
        m_skybox.useBrdfLUT(7);

        // shadowmapping texture
        s.uniform_data("shadowmapTexture", 8);
        m_shadowPass->activateTexture(8);

        s.uniform_data("pointLightsNumber", (int)m_pointLights.size());
        for(int i = 0; i < m_pointLights.size(); i++)
            m_pointLights[i].send_to_shader(s, i);

        s.uniform_data("dirLightsNumber", (int)m_dirLights.size());
        for(int i = 0; i < m_dirLights.size(); i++)
            m_dirLights[i].send_to_shader(s, i);

        m_colorPass->render(m_sceneRoot, *mainCamera, m_shadowPass->m_lightspaceMatrix, s);

        if (drawSkybox) {
            glDepthFunc(GL_LEQUAL);
            s_skybox.use();
            glm::mat4 skyboxView = glm::mat4(glm::mat3(mainCamera->view()));
            s_skybox.uniform_data("viewMatrix", skyboxView);
            s_skybox.uniform_data("projectionMatrix", mainCamera->projection());

            m_skybox.draw();
        }
        Pass* lastTechnique = m_colorPass.get();

        // Post process
        // first, set the last active technique as final
        bool hasActivePostProcess{};
        for(int i = m_PPtechniques.size() - 1; i >= 0; i--)
        {
            if(m_PPtechniques[i]->isActive)
            {
                m_PPtechniques[i]->m_bIsFinal = true;
                hasActivePostProcess = true;
                break;
            }
        }

        // then apply post processing, or just the color pass if no technique is active
        if(!hasActivePostProcess)
            m_colorPass->blitToScreen();
        else
            for(const auto& technique : m_PPtechniques)
            {
                if(!technique->isActive)
                    continue;
                technique->render(lastTechnique);
                lastTechnique = technique.get();
            }

        // general settings
        ImGui::Begin("Rendering");
        if(ImGui::TreeNode("Shadows"))
        {
            if(ImGui::InputFloat("Shadowmap Resolution", &shadowmapResolution))
                m_shadowPass->setResolution(shadowmapResolution);
            if(ImGui::InputFloat("Shadow Zoom", &shadowZoom))
                m_shadowPass->setOrthographicZoom(shadowZoom);

            ImGui::TreePop();
        }
        for(auto& technique : m_PPtechniques)
            technique->drawInspector(mainCamera);
        if(ImGui::Button("Reload shaders"))
        {
            s = Shader("PBR.vs", "PBR.fs");
            s_skybox = Shader("skybox.vs", "skybox.fs");
        }
        ImGui::End();

        ImGui::Begin("Hierarchy");
        m_sceneRoot->drawHierarchy(m_selectedEntity);
        ImGui::End();

        ImGui::Begin("Light");
        for(int i = 0; i < m_dirLights.size(); i++)
        {
            ImGui::PushID(i);
            m_dirLights[i].drawInspector(mainCamera);
            ImGui::PopID();
        }
        for(int i = 0; i < m_pointLights.size(); i++)
        {
            ImGui::PushID(i);
            m_pointLights[i].drawInspector(mainCamera);
            ImGui::PopID();
        }
        ImGui::End();

        ImGui::Begin("Inspector");
            ImGuiIO& io = ImGui::GetIO();
            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
            m_selectedEntity.drawInspector(mainCamera);
            if(pickingFile)
            {
                loadAsset();
            }
        ImGui::End();

        ImGui::Begin("Performances", nullptr,  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
            ImGui::Text("SproutEngine version 0.0.1");
            ImGui::Text("OpenGL%s", api);
            ImGui::Text("%s %s", vendor, gpu);

            float average = 0.0f;
            for(float n : framerate)
                average += n;
            average /= (float)IM_ARRAYSIZE(framerate);
            ImGui::Text("Average %02fFPS", average);

            int milli = (int) (gpu_last_frame / 1000000);
            int micro = (int) ((gpu_last_frame / 1000) % 1000);
            ImGui::Text(
                    "cpu %03dms\ngpu %02dms % 03dus",
                    cpu_last_frame,
                    milli, micro
            );
        ImGui::End();

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
    std::unique_ptr<Entity> m_sceneRoot {std::make_unique<Entity>()};
    Entity m_selectedEntity {};
    Shader s, s_skybox, s_shadowmapping;
    Camera playerCamera;
    HDRCubemap m_skybox;

    std::vector<PointLight> m_pointLights;
    std::vector<DirectionalLight> m_dirLights;

    std::vector<std::unique_ptr<PostProcessPass>> m_PPtechniques;
    std::unique_ptr<ForwardPass> m_colorPass;
    std::unique_ptr<ShadowmappingPass> m_shadowPass;

    // imgui inputs
    //general
    //bool wireframe_mode = false;
    bool drawSkybox = true;
    bool pickingFile = false;

    // shadowmapping
    float shadowmapResolution = 1000.f;
    float shadowZoom = 10.f;

    //framerate management
    float framerate[100] = {};
    int values_offset = 0;
};

int main()
{
    SproutEngineEditor app;
    app.run();

    return 0;
}