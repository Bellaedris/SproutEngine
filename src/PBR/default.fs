#version 460 core

out vec4 FragColor;

in vec3 norm;
in vec4 position;
in vec2 texCoord;
in vec4 cameraPos;
in vec4 lightspacePos;
in mat3 TBN;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normals;

uniform sampler2D shadowmap;

struct DirectionalLight
{
	vec4 direction;

	vec4 ambiant;
	vec4 diffuse;
	vec4 specular;
};
uniform DirectionalLight dirLights[10];

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 color, float shadow)
{
	// diffuse
	vec3 lightDir = normalize(-light.direction.xyz);
	float diff = max(dot(lightDir, normal), 0.f);

	//specular
	vec3 halfwayDir = (lightDir + viewDir) / length(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, normal), 0.), 32.);

	vec3 ambiant = light.ambiant.xyz;
	vec3 diffuse = light.diffuse.xyz * diff;
	vec3 specular = vec3(texture(texture_specular, texCoord)) * spec * light.specular.xyz;

	return (ambiant + (1.0 - shadow) * (diffuse + specular)) * color;
}

struct PointLight
{
	vec4 position;

	vec4 ambiant;
	vec4 diffuse;
	vec4 specular;

	float constant;
	float linear;
	float quadratic;
};

//layout(std140, binding = 1) uniform pointLightsData
//{
//	PointLight pointLights[100];
//};
uniform PointLight pointLights[100];
uniform int pointLightsNumber;

vec3 calculatePointLight(vec3 normal, vec3 viewDir, vec3 color)
{
	vec3 finalColor = vec3(0.f);
	for(int i = 0; i < pointLightsNumber; i++)
	{
		PointLight light = pointLights[i];
		vec3 dir = light.position.xyz - position.xyz;

		// diffuse
		vec3 lightDir = normalize(dir);
		float diff = max(dot(lightDir, normal), 0.f);

		//specular
		vec3 reflect_dir = reflect(-lightDir, normal);
		float spec = pow(max(dot(reflect_dir, viewDir), 0.), 32.);

		vec3 ambiant = light.ambiant.xyz * color;
		vec3 diffuse = light.diffuse.xyz * diff * color;
		vec3 specular = vec3(texture(texture_specular, texCoord)) * spec * light.specular.xyz;

		float dist = length(dir);
		float attenuation = 1.f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

		finalColor += (ambiant + diffuse + specular) * attenuation;
	}

	return finalColor;
}

struct SpotLight
{
	vec3 position;

	vec3 ambiant;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
	float cutoff;
};
uniform SpotLight spotLights[10];

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 color)
{
	if (dot(light.direction, viewDir) > light.cutoff)
	{
		// diffuse
		vec3 lightDir = normalize(light.position - position.xyz);
		float diff = max(dot(lightDir, normal), 0.f);

		//specular
		vec3 reflect_dir = reflect(-lightDir, normal);
		float spec = pow(max(dot(reflect_dir, viewDir), 0.), 32.);

		vec3 ambiant = light.ambiant * color;
		vec3 diffuse = light.diffuse * diff * color;
		vec3 specular = vec3(texture(texture_specular, texCoord)) * spec * light.specular;

		return (ambiant + diffuse + specular);
	}
	else
	{
		return light.ambiant * color;
	}
}

float computeShadow(vec4 lightspace_pos)
{
	//gl_Position does perspective division automatically but not our out param
	vec3 projCoord = lightspace_pos.xyz / lightspace_pos.w;

	projCoord = projCoord * 0.5 + 0.5; // from [-1;1] to [0;1]

	if (projCoord.z > 1.f)
	return 1.f;

	float bias = 0.005;
	// in the shadow if current pixel is in front of shadowmap
	return projCoord.z > texture(shadowmap, projCoord.xy).r + bias ? .9f : 0.f;
}

uniform float gamma;

void main()
{
	vec3 color = texture(texture_diffuse, texCoord).xyz;

	vec3 normal = texture(texture_normals, texCoord).xyz;
	normal = normal * 2.f - 1.f;
	normal = normalize(TBN * normal);

	vec3 viewDir = normalize(cameraPos.xyz - position.xyz);

	float shadow = 0.f; //computeShadow(lightspacePos);

	vec3 finalColor = calculateDirectionalLight(dirLights[0], normal, viewDir, color, shadow);
	finalColor += calculatePointLight(normal, viewDir, color);
	//finalcolor += calculateSpotLight(SpotLight[0], normal, viewDir, color);

	FragColor = vec4(pow(finalColor, vec3(1.f / gamma)), 1.f);
}