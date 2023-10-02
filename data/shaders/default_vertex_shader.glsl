#version 460 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

out vec3 norm;
out vec3 position;
out vec2 texCoord;

uniform mat4 mvpMatrix;

void main()
{
	texCoord = uv;
	norm = normal;
	position = pos;
	gl_Position = mvpMatrix * vec4(pos, 1.f);
}