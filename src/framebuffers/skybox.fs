#version 460

out vec4 fragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
    fragColor = texture(skybox, texCoords);
}