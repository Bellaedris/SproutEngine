#version 460

out vec4 fragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
    vec3 envColor = texture(skybox, texCoords).xyz;

    fragColor = vec4(envColor, 1.f);
}