#version 460 core

out vec4 FragColor;

in vec3 norm;
in vec4 position;
in vec2 texCoord;

uniform sampler2D diffuse_texture1;
uniform sampler2D specular_texture1;

struct Material
{
	vec4 diffuse;
	vec4 specular;
	vec4 emissive;
};
uniform Material mat;

uniform int has_texture;

void main()
{
	vec3 color;
	if (has_texture == 0)
		color = texture(diffuse_texture1, texCoord).xyz;
	else
		color = mat.diffuse.rgb;

	FragColor = vec4(color, 1.f);
}