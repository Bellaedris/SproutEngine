#version 460 core

out vec4 FragColor;

in vec4 worldPosition;
in vec4 cameraPos;
in vec2 texCoord;

uniform sampler2D color_map;
uniform sampler2D height_map;

uniform vec3 sun_pos;
uniform vec2 terrain_size;

uniform int iters = 10000;
uniform float step = 0.001;

void main()
{
	vec3 color = texture(color_map, texCoord).xyz;

	float h = texture(height_map, texCoord).x;
	vec3 pos = vec3(worldPosition.xy, h);

	vec3 sunDir = sun_pos - pos;
	float distanceStep = length(sunDir) / iters;

	for(int i = 0; i < iters; i++)
	{
		pos += sunDir * distanceStep;

		// if our ray is below the ground, we have a hit
		// remember to normalize the position by the terrain size to get UVs
		if (pos.z < texture(height_map, pos.xy / terrain_size).x)
		{
			color *= .5f;
			break;
		}
		if (pos.z > 1.f)
		{
			break;
		}
	}

	//vec3 normal = normalize(norm);
	//vec3 viewDir = normalize(cameraPos.xyz - position.xyz);

	FragColor = vec4(color, 1.f);
}