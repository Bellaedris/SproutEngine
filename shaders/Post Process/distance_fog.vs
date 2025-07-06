#version 460 core

layout(location=0) in vec3 pos;
layout(location=2) in vec2 uv;

uniform mat4 inverseProjectionMatrix;

out vec2 texcoord;
out vec3 cameraToPixelRay;

void main()
{
	texcoord = uv;
	cameraToPixelRay = (inverseProjectionMatrix * vec4(pos, 1.f)).xyz;
	gl_Position = vec4(pos, 1.f);
}