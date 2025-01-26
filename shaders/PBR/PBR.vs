#version 460 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

out vec3 norm;
out vec4 position;
out vec2 texCoord;
out vec4 cameraPos;
out vec4 lightspacePos;
out mat3 TBN;

uniform mat4 normalMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 inverseViewMatrix;
uniform mat4 lightspaceMatrix;

void main()
{
	vec3 T = normalize(vec3(modelMatrix * vec4(tangent,   0.0)));
	vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(modelMatrix * vec4(normal,    0.0)));
	TBN = mat3(T, B, N);
	texCoord = uv;
	norm = (normalMatrix * vec4(normal, 0.f)).xyz;
	position = modelMatrix * vec4(pos, 1.f);
	cameraPos = inverseViewMatrix * vec4(0.f, 0.f, 0.f, 1.f);
	lightspacePos = lightspaceMatrix * vec4(pos, 1.f);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos, 1.f);
}