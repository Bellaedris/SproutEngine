#version 460 core

layout(location=0) in vec3 pos;

uniform mat4 mvpMatrix;

void main()
{
	gl_Position = mvpMatrix * vec4(pos, 1.f);
}