#include <sprout_engine/computeShader.h>
#include <sprout_engine/SproutApp.h>

#include <sprout_engine/model.h>
#include <sprout_engine/texture.h>
#include <sprout_engine/line.h>
#include "sprout_engine/ray_utils/traceableManager.h"
#include <sprout_engine/ray_utils/Traceables/sphere.h>
#include <sprout_engine/ray_utils/RayTracingMaterials/lambertian.h>
#include <sprout_engine/ray_utils/RayTracingMaterials/metallic.h>

std::string resources_path = "../../resources/";


class Raytracer : public SproutApp
{
public:
    Raytracer() : SproutApp(1366, 768, 4, 6) {}

    [[nodiscard]] std::vector<glm::vec4> generateDirections() const
    {
        std::vector<glm::vec4> directions(width() * height());
        for(int j = 0; j < height(); j++)
            for(int i = 0; i < width(); i++)
            {
                directions[width() * j + i] = TraceableManager::generateRayOnHemisphere();
            }
        return directions;
    }

    int init() override
    {
        //read a mesh and create a camera
        m_model = Model(resources_path + "models/cornell-box.obj");
        cam = new Camera({0, 2, 5}, {0, 1, 0}, 0, -90.f, 0.1f, 100.f, 70.f, (float)width() / (float)height());
        m_quad = Mesh::generatePlane();
        m_shader = Shader("texture.vs", "texture.fs");
        m_debugShader = Shader("default.vs", "default.fs");
        m_compute = ComputeShader("raytrace.cs");

        //add traceable objects
        m_traceables.setAspectRatio((float)width() / (float)height());
        m_traceables.setImageWidth(width());

        m_groundMaterial = std::make_shared<Lambertian>(Color(.8f, .8f, .0f, 1.f));
        m_centerMaterial = std::make_shared<Lambertian>(Color(.1f, .2f, .5f, 1.f));
        m_leftMaterial = std::make_shared<Metallic>(Color(.8f, .8f, .8f, 1.f));
        m_rightMaterial = std::make_shared<Metallic>(Color(.8f, .6f, .2f, 1.f));

        m_traceables.add(std::make_unique<Sphere>(glm::vec3(0, 0, -1.2), .5, m_centerMaterial));
        m_traceables.add(std::make_unique<Sphere>(glm::vec3(0, -100.5, -1), 100, m_groundMaterial));
        m_traceables.add(std::make_unique<Sphere>(glm::vec3(-1, 0, -1), .5, m_leftMaterial));
        m_traceables.add(std::make_unique<Sphere>(glm::vec3(1, 0, -1), .5, m_rightMaterial));

        // generate a texture containing points on the edge of a sphere

        m_sphericalCoordsTexture = Texture(width(), height(), generateDirections());
        glBindImageTexture(1, m_sphericalCoordsTexture.get_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        m_texture = Texture(width(), height());
        glBindImageTexture(0, m_texture.get_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        glViewport(0, 0, width(), height());
        glClearColor(.1f, .1f, .1f, 1.f);
        return 0;
    }

    int render() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
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
        if(ImGui::CollapsingHeader("RayTrace settings", true))
        {
            ImGui::InputInt("Samples per pixel", &m_traceables.m_samplesPerPixel);
            ImGui::InputInt("Max bounces per pixel", &m_traceables.m_maxBounces);
            if (ImGui::Button("Reload compute"))
            {
                m_sphericalCoordsTexture = Texture(width(), height(), generateDirections());
                glBindImageTexture(1, m_sphericalCoordsTexture.get_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                m_compute = ComputeShader("raytrace.cs");
            }
        }
        if(ImGui::CollapsingHeader("Rendering", true))
        {
            ImGui::Checkbox("Raytrace", &useRaytrace);
            if(ImGui::Button("RayTrace CPU"))
                m_traceables.render();
        }
        ImGui::End();

        if (useRaytrace)
        {
            //render the image
            m_compute.use();
            m_compute.dispatch(width(), height(), 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            m_texture.use();
            m_sphericalCoordsTexture.use(GL_TEXTURE1);
            m_shader.use();
            m_shader.uniform_data("imageOutput", 0);
            m_shader.uniform_data("sphericalCoords", 1);
            m_quad.draw_strip(m_shader);
        }
        else
        {
            m_debugShader.use();
            m_debugShader.uniform_data("mvpMatrix", cam->projection() * cam->view());

            m_model.draw(m_debugShader);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // update framerate
        framerate[values_offset] = 1.f / delta_time;
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(framerate);

        return 0;
    }

    int quit() override
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return 0;
    }

protected:
    Model m_model;
    Mesh m_quad;
    Shader m_shader;
    Shader m_debugShader;
    ComputeShader m_compute;
    Texture m_texture;
    Texture m_sphericalCoordsTexture;

    // materials
    std::shared_ptr<Lambertian> m_groundMaterial;
    std::shared_ptr<Lambertian> m_centerMaterial;
    std::shared_ptr<Metallic> m_leftMaterial;
    std::shared_ptr<Metallic> m_rightMaterial;

    TraceableManager m_traceables;

    bool useRaytrace{true};

    float framerate[90] = {};
    int values_offset = 0;
};

int main()
{
    Raytracer app;
    app.run();

    return 0;
}