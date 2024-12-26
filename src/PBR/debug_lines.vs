#version 460 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;

out vec3 col;

uniform mat4 mvpMatrix;

void main()
{
	col = color;
	gl_Position = mvpMatrix * vec4(pos, 1.f);
}