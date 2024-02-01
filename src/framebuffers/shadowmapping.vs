#version 460 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

uniform mat4 mvpMatrix;

void main()
{
	gl_Position = mvpMatrix * vec4(pos, 1.f);
}