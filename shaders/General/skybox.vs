#version 460

layout(location = 0) in vec3 position;

out vec3 texCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    texCoords = vec3(position.x, position.y, position.z);
    vec4 pos = projectionMatrix * viewMatrix * vec4(position, 1.f);
    gl_Position = pos.xyww; // trick the perspective division: z is always 1, so always on top
}