#version 460 core

#define M_PI 3.1415926535897932384626433832795

out vec4 FragColor;

in vec3 norm;
in vec4 position;
in vec2 texCoord;
in vec4 cameraPos;
in vec4 lightspacePos;
in mat3 TBN;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_roughnessMetalness;
uniform sampler2D texture_normals;
uniform sampler2D texture_emissive;
uniform sampler2D texture_ao;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

struct SurfaceParameter
{
	vec3 albedo;
	float metalness;
	vec3 normal;
	float roughness;
	vec3 F0;
};

// PBR functions
float distributionGGX(in vec3 normal, in vec3 halfway, in float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float nDotH = max(dot(normal, halfway), 0.f);

	float top = a2;
	float bottom = nDotH * nDotH * (a2 - 1.f) + 1.f;
	bottom = M_PI * bottom * bottom;

	return top / bottom;
}

vec3 schlickFresnel(float cosTheta, vec3 F0)
{
	return F0 + (1.f - F0) * pow(clamp(1.f - cosTheta, 0.f, 1.f), 5.f);
}

// from https://seblagarde.wordpress.com/2011/08/17/hello-world/, corrected fresnel for our ambiant light, that does take roughness into account
vec3 schlickFresnelRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.f - roughness), F0) - F0) * pow(clamp(1.f - cosTheta, .0f, 1.f), 5.f);
}

float geometrySclickGGX(in float normDotDir, in float k)
{
	return normDotDir / (normDotDir * (1.f - k) + k);
}

float SmithGGX(in vec3 normal, in vec3 viewDir, in vec3 lightDir, in float roughness)
{
	// k is a remapping of roughness depending on if we're sampling ibl or direct lighting
	float r = roughness + 1.f;
	float k = (r * r) / 8.f;

	return
	geometrySclickGGX(max(dot(normal, viewDir), 0), k)
	* geometrySclickGGX(max(dot(normal, lightDir), 0), k);
}

struct DirectionalLight
{
	vec3 direction;
	float intensity;
	vec4 color;
};
uniform DirectionalLight dirLights[10];
uniform int dirLightsNumber;

vec3 calculateDirectionalLight(vec3 viewDir, SurfaceParameter surface)
{
	vec3 Lo = vec3(0.f);
	for(int i = 0; i < dirLightsNumber; i++)
	{
		DirectionalLight light = dirLights[i];
		vec3 L = normalize(-light.direction);
		vec3 H = normalize(L + viewDir);

		vec3 radiance = light.color.xyz * light.intensity;

		float cosTheta = max(dot(viewDir, H), 0);

		float D = distributionGGX(surface.normal, H, surface.roughness);
		vec3 F = schlickFresnel(cosTheta, surface.F0);
		float G = SmithGGX(surface.normal, viewDir, L, surface.roughness);

		vec3 top = D * F * G;
		float bottom = 4.f * max(dot(viewDir, surface.normal), 0) * max(dot(L, surface.normal), 0) + .0001f; // avoid div by zero...
		vec3 specular = top / bottom;

		vec3 Ks = F;
		vec3 Kd = vec3(1.f) - Ks;
		Kd *= 1.f - surface.metalness;

		float NdotL = max(dot(surface.normal, L), 0.f);
		Lo += (Kd * surface.albedo / M_PI + specular) * radiance * NdotL;
	}

	return Lo;
}

struct PointLight
{
	vec3 position;
	float intensity;
	vec4 color;
};

//layout(std140, binding = 1) uniform pointLightsData
//{
//	PointLight pointLights[100];
//};
uniform PointLight pointLights[100];
uniform int pointLightsNumber;

vec3 calculatePointLight(vec3 viewDir, SurfaceParameter surface)
{
	vec3 Lo = vec3(0.f);
	for(int i = 0; i < pointLightsNumber; i++)
	{
		PointLight light = pointLights[i];
		vec3 L = normalize(light.position - position.xyz);
		vec3 H = normalize(L + viewDir);

		float dist = length(light.position - position.xyz);
		float attenuation = 1.f / (dist * dist);
		vec3 radiance = light.color.xyz * attenuation * light.intensity;

		float cosTheta = max(dot(viewDir, H), 0);

		float D = distributionGGX(surface.normal, H, surface.roughness);
		vec3 F = schlickFresnel(cosTheta, surface.F0);
		float G = SmithGGX(surface.normal, viewDir, L, surface.roughness);

		vec3 top = D * F * G;
		float bottom = 4.f * max(dot(viewDir, surface.normal), 0) * max(dot(L, surface.normal), 0) + .0001f; // avoid div by zero...
		vec3 specular = top / bottom;

		vec3 Ks = F;
		vec3 Kd = vec3(1.f) - Ks;
		Kd *= 1.f - surface.metalness;

		float NdotL = max(dot(surface.normal, L), 0.f);
		Lo += (Kd * surface.albedo / M_PI + specular) * radiance * NdotL;
	}

	return Lo;
}

//struct SpotLight
//{
//	vec3 position;
//
//	vec3 ambiant;
//	vec3 diffuse;
//	vec3 specular;
//
//	vec3 direction;
//	float cutoff;
//};
//uniform SpotLight spotLights[10];
//
//vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 color)
//{
//	if (dot(light.direction, viewDir) > light.cutoff)
//	{
//		// diffuse
//		vec3 lightDir = normalize(light.position - position.xyz);
//		float diff = max(dot(lightDir, normal), 0.f);
//
//		//specular
//		vec3 reflect_dir = reflect(-lightDir, normal);
//		float spec = pow(max(dot(reflect_dir, viewDir), 0.), 32.);
//
//		vec3 ambiant = light.ambiant * color;
//		vec3 diffuse = light.diffuse * diff * color;
//		vec3 specular = vec3(texture(texture_specular, texCoord)) * spec * light.specular;
//
//		return (ambiant + diffuse + specular);
//	}
//	else
//	{
//		return light.ambiant * color;
//	}
//}

//float computeShadow(vec4 lightspace_pos)
//{
//	//gl_Position does perspective division automatically but not our out param
//	vec3 projCoord = lightspace_pos.xyz / lightspace_pos.w;
//
//	projCoord = projCoord * 0.5 + 0.5; // from [-1;1] to [0;1]
//
//	if (projCoord.z > 1.f)
//	return 1.f;
//
//	float bias = 0.005;
//	// in the shadow if current pixel is in front of shadowmap
//	return projCoord.z > texture(shadowmap, projCoord.xy).r + bias ? .9f : 0.f;
//}

uniform float gamma;

void main()
{
	vec3 albedo = texture(texture_diffuse, texCoord).xyz;
	vec3 emissive = texture(texture_emissive, texCoord).xyz;
	float metalness = texture(texture_roughnessMetalness, texCoord).b;
	float roughness = texture(texture_roughnessMetalness, texCoord).g;
	float ao = texture(texture_ao, texCoord).r;

	vec3 normal = texture(texture_normals, texCoord).xyz;
	normal = normal * 2.f - 1.f;
	normal = normalize(TBN * normal);

	vec3 viewDir = normalize(cameraPos.xyz - position.xyz);

	// compute IBL diffuse/specular parts
	//diffuse
	float cosTheta = max(dot(viewDir, normal), 0);
	vec3 F0 = vec3(.04f);
	F0 = mix(F0, albedo, metalness);

	// specular
	vec3 R = reflect(-viewDir, normal);
	const float MAX_REFLEXION_LOD = 4.0; // replace with the amount of mips in the prefiltered map
	vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLEXION_LOD).xyz;

	vec3 F = schlickFresnelRoughness(max(dot(normal, viewDir), .0f), F0, roughness);
	vec2 envBRDF = texture(brdfLUT, vec2(max(dot(normal, viewDir), .0f), roughness)).xy;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 kS = F;
	vec3 kD = 1.f - kS;
	kD *= 1.f - metalness;

	vec3 irradiance = texture(irradianceMap, normal).xyz;
	vec3 diffuse = irradiance * albedo * ao;
	vec3 ambiant = (kD * diffuse + specular) * ao;

	//float shadow = computeShadow(lightspacePos);

	// pack all our data in a struct
	SurfaceParameter surface = SurfaceParameter(albedo, metalness, normal, roughness, F0);
	vec3 finalColor = calculateDirectionalLight(viewDir, surface);
	finalColor += calculatePointLight(viewDir, surface);
	//finalcolor += calculateSpotLight(SpotLight[0], normal, viewDir, color);

	finalColor += ambiant;
	finalColor += emissive;
	finalColor = finalColor / (finalColor + vec3(1.f));

	FragColor = vec4(finalColor, 1.f);
}