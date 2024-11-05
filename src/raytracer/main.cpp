#include <sprout_engine/buffer.h>
#include <sprout_engine/computeShader.h>
#include <sprout_engine/light.h>
#include <sprout_engine/SproutApp.h>

#include <sprout_engine/model.h>
#include <sprout_engine/texture.h>
#include <sprout_engine/line.h>
#include <sprout_engine/utils.h>
#include <sprout_engine/ray_utils/bvhNode.h>
#include <sprout_engine/ray_utils/RayTracingMaterials/dielectrics.h>

#include "sprout_engine/ray_utils/traceableManager.h"
#include <sprout_engine/ray_utils/Traceables/sphere.h>
#include <sprout_engine/ray_utils/RayTracingMaterials/lambertian.h>
#include <sprout_engine/ray_utils/RayTracingMaterials/metallic.h>

std::string resources_path = "../../resources/";

struct TriangleRay
{
    glm::vec4 a;
    glm::vec4 b;
    glm::vec4 c;
    glm::vec4 na;
    glm::vec4 nb;
    glm::vec4 nc;
    int materialId;
};

struct LightsRay
{
    glm::vec4 ambiant;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec4 dir;
};

struct MaterialRay
{
    glm::vec4 diffuse;
    glm::vec4 emissive;
};

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
        mainCamera = new Camera({0, 0, 0}, {0, 1, 0}, 0, -90.f, 0.1f, 100.f, 70.f, (float)width() / (float)height());
        m_quad = Mesh::generatePlane();
        m_shader = Shader("texture.vs", "texture.fs");
        m_debugShader = Shader("default.vs", "default.fs");
        m_AABBDisplay = Shader("debug_lines.vs", "debug_lines.fs");
        m_compute = ComputeShader("raytrace.cs");

        //add traceable objects
        m_traceables.setCamera(mainCamera);
        m_traceables.setAspectRatio((float)width() / (float)height());
        m_traceables.setImageWidth(width());

        m_traceablesBVH.setCamera(mainCamera);
        m_traceablesBVH.setAspectRatio((float)width() / (float)height());
        m_traceablesBVH.setImageWidth(width());

        m_groundMaterial = std::make_shared<Lambertian>(Color(.8f, .8f, .0f, 1.f));
        m_centerMaterial = std::make_shared<Lambertian>(Color(.1f, .2f, .5f, 1.f));
        m_outerLeftMaterial = std::make_shared<Dielectric>(1.5f);
        // the sphere is made of air in a world of glass
        m_innerLeftMaterial = std::make_shared<Dielectric>(1.f / 1.5f);
        m_rightMaterial = std::make_shared<Metallic>(Color(.8f, .6f, .2f, 1.f), 1.);

        std::vector<Traceable*> traceables;
        // traceables.push_back(new Box(glm::vec3(-.25, -.25, -1.45), glm::vec3(.25, .25, -0.95), m_centerMaterial)); // box centered at 0, 0, -1.2
        //traceables.push_back(new Sphere(glm::vec3(0, 0, -1), .4, m_centerMaterial));
        // traceables.push_back(new Sphere(glm::vec3(0, -100.5, -1), 100, m_groundMaterial));
        //traceables.push_back(new Sphere(glm::vec3(-1, 0, -1), .4, m_innerLeftMaterial));
        //traceables.push_back(new Sphere(glm::vec3(-1, 0, -1), .5, m_outerLeftMaterial));
        //traceables.push_back(new Sphere(glm::vec3(1, 0, -1), .5, m_rightMaterial));

        for(int i = 0; i < 100; i++)
        {
            traceables.push_back(new Sphere(glm::vec3(Utils::randomRange(0, 10) - 5.f, Utils::randomRange(0, 1), -Utils::randomRange(1, 10)), Utils::randomRange(.05, .2), m_groundMaterial));
        }

        m_trianglesData.Bind(2);
        m_matData.Bind(4);

        std::vector<MaterialRay> materials;

        // read all triangles to build the data struture
        for(const auto& m : m_model.getMeshes())
        {
            const auto& vertices = m.positions();
            const auto& indices = m.indices();
            const auto& normals = m.normals();
            const auto& mat = m.mat();

            for(int i = 0; i < m.indices().size(); i+=3)
            {
                m_triangles.push_back({
                    {vertices[indices[i]].x, vertices[indices[i]].y, vertices[indices[i]].z, 1.f},
                    {vertices[indices[i + 1]].x, vertices[indices[i + 1]].y, vertices[indices[i + 1]].z, 1.f},
                    {vertices[indices[i + 2]].x, vertices[indices[i + 2]].y, vertices[indices[i + 2]].z, 1.f},

                {normals[indices[i]].x, normals[indices[i]].y, normals[indices[i]].z, 1.f},
                {normals[indices[i + 1]].x, normals[indices[i + 1]].y, normals[indices[i + 1]].z, 1.f},
                {normals[indices[i + 2]].x, normals[indices[i + 2]].y, normals[indices[i + 2]].z, 1.f},
                    }
                );
            }

            materials.push_back(
            {
                mat.diffuse,
                mat.emissive
            });
        }
        m_trianglesData.Allocate(100);
        m_trianglesData.Update(m_triangles);

        // add the materials inked to the mesh
        m_matData.Allocate(50);
        m_matData.Update(materials);

        // add a few lights and init our ubo
        m_lightsData.Bind(3);
        m_lightsData.Allocate(50);

        auto start = std::chrono::high_resolution_clock::now();

        auto world = new BVHNode(traceables, 0, traceables.size(), 0, 20);

        auto bvhEnd = std::chrono::high_resolution_clock::now();

        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(bvhEnd - start).count() << "ms" << std::endl;

        m_traceables.add(world);
        m_traceablesBVH.add(traceables);

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
        ImGui::Begin("Inspector");
            if(ImGui::Button("Add dir light"))
                m_lights.emplace_back();
            int index = 0;
            for (auto& light : m_lights)
            {
                index += 1;
                light.setName(index);
                light.drawInspector();
            }
        ImGui::End();

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
            if(ImGui::Button("Update dir light"))
            {
                std::vector<LightsRay> m_gpuLights;
                int index = 0;
                for(auto& light : m_lights)
                {
                    m_gpuLights.push_back({
                        light.GetAmbiant(),
                        light.GetDiffuse(),
                        light.GetSpecular(),
                        light.getDirection()
                        }
                    );
                    index++;
                }
                m_lightsData.Update(m_gpuLights);
            }

            if(ImGui::InputInt("Samples per pixel", &samplesPerPixel))
            {
                m_traceables.setSamplesPerPixel(samplesPerPixel);
            };
            if(ImGui::InputInt("Max bounces per pixel", &bouncesMaxPerRay))
            {
                m_traceables.setMaxBounces(bouncesMaxPerRay);
            }
            ImGui::SliderInt("Maximal BVH depth displayed", &BVHMaxDisplayDepth, 0, 30);
            ImGui::Checkbox("Use BVH", &useBVH);
            if (ImGui::Button("Reload compute"))
            {
                m_sphericalCoordsTexture = Texture(width(), height(), generateDirections());
                glBindImageTexture(1, m_sphericalCoordsTexture.get_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                m_compute = ComputeShader("raytrace.cs");
            }
        }
        if(ImGui::CollapsingHeader("Rendering", true))
        {
            ImGui::Checkbox("Raytrace GPU", &useRaytrace);
            if(ImGui::Button("RayTrace CPU"))
            {
                auto start = std::chrono::high_resolution_clock::now();
                if (useBVH)
                    m_traceables.render();
                else
                    m_traceablesBVH.render();
                auto renderEnd = std::chrono::high_resolution_clock::now();

                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(renderEnd - start).count() << "ms" << std::endl;
            }
        }
        ImGui::End();

        if (useRaytrace)
        {
            //render the image
            m_sphericalCoordsTexture.use(GL_TEXTURE1);
            m_compute.use();

            // send texture datas
            m_compute.uniform_data("imageOutput", 0);
            m_compute.uniform_data("sphericalCoords", 1);
            //send camera datas
            m_compute.uniform_data("fov", mainCamera->getFov());
            m_compute.uniform_data("cameraPos", mainCamera->get_position());
            m_compute.uniform_data("aspectRatio", mainCamera->getAspectRatio());
            m_compute.uniform_data("camDir", mainCamera->getDir());
            m_compute.uniform_data("camRight", mainCamera->getRight());
            m_compute.uniform_data("camUp", mainCamera->getUp());

            m_compute.uniform_data("spp", samplesPerPixel);
            m_compute.uniform_data("numberOfTriangles", static_cast<int>(m_triangles.size()));
            m_compute.uniform_data("numberOfLights", static_cast<int>(m_lights.size()));

            m_compute.dispatch(width(), height(), 1);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            // blit to screen
            m_shader.use();
            m_texture.use();
            m_quad.draw_strip(m_shader);
        }
        else
        {
            //m_debugShader.use();
           // m_debugShader.uniform_data("mvpMatrix", cam->projection() * cam->view());

            //m_model.draw(m_debugShader);

            m_AABBDisplay.use();
            m_AABBDisplay.uniform_data("mvpMatrix", mainCamera->projection() * mainCamera->view());
            m_traceables.drawBoundingBoxes(m_AABBDisplay, BVHMaxDisplayDepth);
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
    Shader m_AABBDisplay;
    ComputeShader m_compute;
    Texture m_texture;
    Texture m_sphericalCoordsTexture;
    UniformBuffer<TriangleRay> m_trianglesData;
    UniformBuffer<LightsRay> m_lightsData;
    UniformBuffer<MaterialRay> m_matData;

    std::vector<TriangleRay> m_triangles;
    std::vector<DirectionalLight> m_lights;

    // materials
    std::shared_ptr<Lambertian> m_groundMaterial;
    std::shared_ptr<Lambertian> m_centerMaterial;
    std::shared_ptr<Dielectric> m_outerLeftMaterial;
    std::shared_ptr<Dielectric> m_innerLeftMaterial;
    std::shared_ptr<Metallic> m_rightMaterial;

    TraceableManager m_traceables;
    TraceableManager m_traceablesBVH;

    bool useRaytrace{true};
    bool useBVH{true};
    int samplesPerPixel = 1;
    int bouncesMaxPerRay = 10;
    int BVHMaxDisplayDepth = 1;

    float framerate[90] = {};
    int values_offset = 0;
};

int main()
{
    Raytracer app;
    app.run();

    return 0;
}