#version 460 core

out vec4 FragColor;

in vec3 norm;
in vec4 position;
in vec2 texCoord;
in vec4 cameraPos;

struct LightSource
{
	vec3 position;

	vec3 ambiant;
	vec3 diffuse;
	vec3 specular;
};

uniform LightSource light;
uniform sampler2D diffuse_texture1;
uniform sampler2D specular_texture1;

void main()
{
	vec3 color = texture(diffuse_texture1, texCoord).xyz;
	vec3 ambiant = light.ambiant * 0.1f;

	vec3 normal = normalize(norm);
	
	//diffuse color: angle between the vector position-light and the normal
	vec3 lightDir = normalize(light.position - position.xyz);
	float cos_theta = max(dot(lightDir, normal), 0.f);
	vec3 diffuse = light.diffuse * cos_theta;

	// specular: angle between the position and the reflection of the light
	vec3 viewDir = normalize(cameraPos.xyz - position.xyz);
	vec3 reflect_dir = reflect(-lightDir, normal);
	float specular = pow(max(dot(reflect_dir, viewDir), 0.), 32.);
	vec3 specular_color = vec3(texture(specular_texture1, texCoord)) * specular * light.specular;

	//FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), .2);
	FragColor = vec4(color * (ambiant + diffuse + specular), 1.f);
}