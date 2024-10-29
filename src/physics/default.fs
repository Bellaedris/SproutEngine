#version 460 core

out vec4 FragColor;

in vec3 norm;
in vec4 position;
in vec2 texCoord;
in vec4 cameraPos;

uniform sampler2D diffuse_texture1;
uniform sampler2D specular_texture1;

uniform int has_texture;

struct Material
{
	vec4 diffuse;
	vec4 specular;
	vec4 emissive;
};
uniform Material mat;

struct DirectionalLight
{
	vec4 direction;

	vec4 ambiant;
	vec4 diffuse;
	vec4 specular;
};
uniform DirectionalLight dirLights[10];

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 color)
{
	// diffuse
	vec3 lightDir = normalize(-light.direction.xyz);
	float diff = max(dot(lightDir, normal), 0.f);

	//specular
	vec3 reflect_dir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflect_dir, viewDir), 0.), 32.);

	vec3 ambiant = light.ambiant.xyz;
	vec3 diffuse = light.diffuse.xyz * diff;
	vec3 specular = vec3(texture(specular_texture1, texCoord)) * spec * light.specular.xyz;

	return (ambiant + (diffuse + specular)) * color;
}

void main()
{
	vec3 color;
	if (has_texture == 0)
	color = texture(diffuse_texture1, texCoord).xyz;
	else
	color = mat.diffuse.rgb;

	vec3 normal = normalize(norm);
	vec3 viewDir = normalize(cameraPos.xyz - position.xyz);

	vec3 finalColor = calculateDirectionalLight(dirLights[0], normal, viewDir, color);

	FragColor = vec4(finalColor, 1.f);
}