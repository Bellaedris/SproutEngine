#version 460 core

out vec4 FragColor;

in vec3 norm;
in vec4 position;
in vec2 texCoord;
in vec4 cameraPos;

uniform sampler2D diffuse_texture1;
uniform sampler2D specular_texture1;

struct DirectionalLight
{
	vec3 direction;

	vec3 ambiant;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight light;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 color)
{
	// diffuse
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(lightDir, normal), 0.f);

	//specular
	vec3 reflect_dir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflect_dir, viewDir), 0.), 32.);

	vec3 ambiant = light.ambiant * color;
	vec3 diffuse = light.diffuse * diff * color;
	vec3 specular = vec3(texture(specular_texture1, texCoord)) * spec * light.specular;

	return (ambiant + diffuse + specular);
}

/*struct PointLight
{
    
};

struct SpotLight
{

};*/

void main()
{
	vec3 color = texture(diffuse_texture1, texCoord).xyz;
	vec3 ambiant = light.ambiant * 0.1f;

	vec3 normal = normalize(norm);
	vec3 viewDir = normalize(cameraPos.xyz - position.xyz);

	vec3 finalColor = calculateDirectionalLight(light, normal, viewDir, color);

	//FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), .2);
	FragColor = vec4(finalColor, 1.f);
}