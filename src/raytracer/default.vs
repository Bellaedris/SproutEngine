#version 460 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

out vec3 norm;
out vec4 position;
out vec2 texCoord;

uniform mat4 normalMatrix;
uniform mat4 modelMatrix;
uniform mat4 mvpMatrix;

// HANDLE NORMAL MATRIX WHEN APPLYING A UNIFORM SCALE
// take transpose of inverse of model applied to normal

void main()
{
	texCoord = uv;
	norm = mat3(normalMatrix) * normal; // should be computed on the CPU
	position = modelMatrix * vec4(pos, 1.f);
	gl_Position = mvpMatrix * vec4(pos, 1.f);
}