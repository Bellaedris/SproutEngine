#version 460 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

out vec3 norm;
out vec4 worldPosition;
out vec4 cameraPos;
out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 inverseViewMatrix;

// HANDLE NORMAL MATRIX WHEN APPLYING A UNIFORM SCALE
// take transpose of inverse of model applied to normal

void main()
{
	texCoord = uv;
	worldPosition = modelMatrix * vec4(pos, 1.f);
	cameraPos = inverseViewMatrix * vec4(0.f, 0.f, 0.f, 1.f);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.f);
}