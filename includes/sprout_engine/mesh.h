#pragma once

#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "bounds.h"
#include "material.h"
#include "PBRMaterial.h"

#include <glm/glm.hpp>
#include <vector>

#include <assimp/aabb.h>
#include <set>
#include <unordered_set>

class Mesh
{
protected:
	std::vector<glm::vec3> m_positions; 
	std::vector<glm::vec3> m_normals;
    std::vector<glm::vec4> m_colors;
	std::vector<glm::vec2> m_texcoords;
    std::vector<glm::vec3> m_tangents;
    std::vector<glm::vec3> m_bitangents;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
    Material m_material;
    PBRMaterialPtr m_pbrMat;

	AABB aabb;

	unsigned int buffer;
	unsigned int index_buffer;
	unsigned int vao;

	bool update_data;

    void computeBitengants();

    /**
	 * \brief Builds the buffer containing the mesh data. Since data will have to go from CPU to GPU,
	 * it may be ineficient and will require a refactor, for instance using glBufferStorage...
	 */
    void build_buffer();

public:
	Mesh() : update_data(false)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &buffer);
		glGenBuffers(1, &index_buffer);
	};

    Mesh(
            const std::vector<glm::vec3> &vertices,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec2>& texcoords,
            const std::vector<glm::vec4>& colors,
            const std::vector<unsigned int> &indices,
            const std::vector<Texture> &textures,
            const PBRMaterialPtr& material,
            const aiAABB &in_aabb
    )
            : update_data(false), m_positions(vertices), m_normals(normals), m_texcoords(texcoords), m_colors(colors), m_indices(indices), m_textures(textures), m_pbrMat(material)
    {
        // Mesh buffers/vao
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &buffer);
        glGenBuffers(1, &index_buffer);

        computeBitengants();
        build_buffer();

        //init AABB
        aabb = AABB(glm::vec3(in_aabb.mMin.x, in_aabb.mMin.y, in_aabb.mMin.z), glm::vec3(in_aabb.mMax.x, in_aabb.mMax.y, in_aabb.mMax.z));

        glBindVertexArray(0);
    };

    // read a Mesh from file. Uses simpler material for RT workflow.
	Mesh(
            const std::vector<glm::vec3> &vertices,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec2>& texcoords,
            const std::vector<glm::vec4>& colors,
            const std::vector<unsigned int> &indices,
            const std::vector<Texture> &textures,
            const Material& material,
            const aiAABB &in_aabb
            )
		: update_data(false), m_positions(vertices), m_normals(normals), m_texcoords(texcoords), m_colors(colors), m_indices(indices), m_textures(textures), m_material(material)
	{
		// Mesh buffers/vao
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &buffer);
		glGenBuffers(1, &index_buffer);

        computeBitengants();
		build_buffer();

		//init AABB
		aabb = AABB(glm::vec3(in_aabb.mMin.x, in_aabb.mMin.y, in_aabb.mMin.z), glm::vec3(in_aabb.mMax.x, in_aabb.mMax.y, in_aabb.mMax.z));

		glBindVertexArray(0);
	};

    // build a Mesh from vertices and texcoords data
    Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2>& texcoords)
            : update_data(false), m_positions(vertices), m_texcoords(texcoords)
    {
        // Mesh buffers/vao
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &buffer);
        glGenBuffers(1, &index_buffer);

        build_buffer();
    };

    // build a Mesh from vertices data
    Mesh(const std::vector<glm::vec3> &vertices)
            : update_data(false), m_positions(vertices)
    {
        // Mesh buffers/vao
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &buffer);
        glGenBuffers(1, &index_buffer);

        build_buffer();
    };

	//read-only accessors
	const std::vector<glm::vec3>& positions() const { return m_positions; };
	const std::vector<glm::vec3>& normals() const { return m_normals; };
    const std::vector<glm::vec4>& colors() const { return m_colors; };
	const std::vector<glm::vec2>& texcoords() const { return m_texcoords; };
	const std::vector<unsigned int>& indices() const { return m_indices; };

    const glm::vec3& position(int p_index) const { return m_positions[p_index]; };
    const glm::vec3& normal(int p_index) const { return m_normals[p_index]; };
    const glm::vec4& color(int p_index) const { return m_colors[p_index]; };
    const glm::vec2& texcoord(int p_index) const { return m_texcoords[p_index]; };
    const unsigned int& indice(int p_index) const { return m_indices[p_index]; };

    inline size_t vertex_buffer_size() const { return m_positions.size() * sizeof(vec3); };
    inline size_t normal_buffer_size() const { return m_normals.size() * sizeof(vec3); };
    inline size_t tangent_buffer_size() const { return m_tangents.size() * sizeof(vec3); };
    inline size_t bitangent_buffer_size() const { return m_bitangents.size() * sizeof(vec3); };
    inline size_t color_buffer_size() const { return m_colors.size() * sizeof(vec4); };
    inline size_t texcoords_buffer_size() const { return m_texcoords.size() * sizeof(vec2); };
    inline size_t indices_buffer_size() const { return m_indices.size() * sizeof(unsigned int); };

	/**
	 * \brief inserts a new vertex at the end of the array
	 */
	void vertex(glm::vec3 pos)
	{
		m_positions.push_back(pos);
		update_data = true;
	}

	/**
	 * \brief inserts a new normal at the end of the array
	 */
	void normal(glm::vec3 norm)
	{
		m_normals.push_back(norm);
		update_data = true;
	}

	/**
	 * \brief inserts a new texture coordinate at the end of the array
	 */
	void texcoord(glm::vec2 uv)
	{
		m_texcoords.push_back(uv);
		update_data = true;
	}

    /**
	 * \brief inserts a new color at the end of the array
	 */
    void color(glm::vec4 color)
    {
        m_colors.push_back(color);
        update_data = true;
    }

	/**
	 * \brief inserts the indices of a triangle in the structure using 3 vertex indices
	 */
	void triangle(int a, int b, int c)
	{
		m_indices.push_back(a);
		m_indices.push_back(b);
		m_indices.push_back(c);
	}

	// all the different draws
	void draw(Shader& s)
	{
		if (update_data)
			build_buffer();

		s.use();

        // if there is no texture, use the material
        if (m_textures.empty())
        {
            s.uniform_data("has_texture", 1);
            s.uniform_data("mat.diffuse", m_material.diffuse);
        }
        else {
            std::unordered_set<std::string> types {"texture_diffuse", "texture_diffuse", "texture_roughnessMetalness", "texture_normals", "texture_emissive", "texture_ao"};
            //render using the available textures
            s.uniform_data("has_texture", 0);
            for (int i = 0; i < m_textures.size(); i++) {
                std::string name = m_textures[i].type;
                types.erase(name);

                m_textures[i].use(GL_TEXTURE0 + i);
                s.uniform_data(name, i); // give the correct location to each texture
            }
            for(const auto& type : types)
                s.uniform_data(type, -1); // deactivate unused texture types

            // deactivate all texture units that are not present. Since we only have 6 texture targets, it is simple,
            // but will have to move if more textures appear
            for(int i = m_textures.size(); i < 6; i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glActiveTexture(GL_TEXTURE0);
        }

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	};

	void draw_unindexed();

    void draw_strip();

	void draw(Shader &s, const Frustum &frustum, const Transform& transform)
	{
		//check if the aabb is visible (inside the frustum)
		//convert aabb to center-extents
		if (!aabb.isOnFrustum(frustum, transform))
			return;
		else
			draw(s);
	};

	void draw_debug_frustum(Shader& s, const Frustum& frustum, const Transform& transform)
	{
		s.use();

		// test collision and give the correct color
		Plane p(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
		if (aabb.isOnFrustum(frustum, transform))
		{
			s.uniform_data("color", 0.f, 1.f, 0.f);
		}
		else
		{
			s.uniform_data("color", 1.f, 0.f, 0.f);
		}

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	};

    /*!
     * Generates a plane that takes the size of the frustum
     * @return a mesh representing a plane from [-1;-1] to [1;1]
     */
    static Mesh generatePlane();

    static Mesh generateCube();
};