#pragma once

#include "texture.h"

#include <array>

class HDRCubemap
{
protected:
    GLuint id;
    Texture hdrTexture;
    Texture cubemap;
    Texture irradiance;
    Mesh cube;
    Shader equirectangularToCubemap;
    Shader irradiancePrecompute;

public:
    HDRCubemap() : id(-1) {};

    HDRCubemap(const char* path)
        : equirectangularToCubemap("equirectangularToCubemap.vs", "equirectangularToCubemap.fs")
        , irradiancePrecompute("equirectangularToCubemap.vs", "cubemapConvolution.fs")
        , cube(Mesh::generateCube())
        , hdrTexture(Texture::loadHDRTexture(path))
    {
        // framebuffer we will draw the cubemap on
        unsigned int captureFBO, captureRBO;
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glNamedRenderbufferStorage(captureRBO, GL_DEPTH_COMPONENT24, 512, 512);
        glNamedFramebufferRenderbuffer(captureFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        // generate the cubemap textures
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.get_id());
        for(int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        // convert HDR equirectangular environment map to cubemap equivalent
        equirectangularToCubemap.use();
        equirectangularToCubemap.uniform_data("equirectangularMap", 0);
        equirectangularToCubemap.uniform_data("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture.get_id());

        glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (uint32_t i = 0; i < 6; ++i)
        {
            equirectangularToCubemap.uniform_data("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap.get_id(), 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cube.draw_unindexed();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // precompute the irradiance
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance.get_id());
        for(uint32_t i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        irradiancePrecompute.use();
        irradiancePrecompute.uniform_data("hdrMap", 0);
        equirectangularToCubemap.uniform_data("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.get_id());

        glViewport(0, 0, 32, 32);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        for (uint32_t i = 0; i < 6; ++i)
        {
            irradiancePrecompute.uniform_data("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance.get_id(), 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cube.draw_unindexed();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void draw()
    {
        glDepthMask(GL_FALSE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.get_id());
        cube.draw_unindexed();
        glDepthMask(GL_TRUE);
    }

    void useIrradiance(uint32_t textureUnit)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance.get_id());
    }
};