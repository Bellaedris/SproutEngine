#version 460 core

out vec4 FragColor;

in vec4 worldPosition;
in vec4 cameraPos;
in vec2 texCoord;
in vec3 norm;

uniform sampler2D color_map;
uniform sampler2D height_map;
uniform sampler2D slope_map;

uniform vec3 sun_dir;
uniform vec2 terrain_size;
uniform float groundSlope;
uniform float maxHeight;

uniform int iters = 1000;
uniform float step = 0.001f;

float remap(float v, float in_start, float in_end, float out_start, float out_end)
{
	float slope = (out_end - out_start) / (in_end - in_start);
	return out_start - slope * (v - in_start);
}

vec3 bilinear(sampler2D tex, vec2 uv)
{
	ivec2 size = textureSize(tex, 0);
	vec2 texelSize = 1.f / size;

	vec2 positionInImage = uv * vec2(size);
	vec2 posInTexel = fract(positionInImage);

	vec3 offsets = vec3(texelSize.x, texelSize.y, 0.f);

	vec3 tl = texture(tex, uv).xyz;
	vec3 tr = texture(tex, uv + offsets.xz).xyz;
	vec3 bl = texture(tex, uv + offsets.zy).xyz;
	vec3 br = texture(tex, uv + offsets.xy).xyz;

	vec3 t = mix(tl, tr, posInTexel.x);
	vec3 b = mix(bl, br, posInTexel.x);

	return mix(t, b, posInTexel.y);
}

void main()
{
	vec3 color = texture(color_map, texCoord).xyz;
	vec3 sunDir = normalize(sun_dir);

	float bias = 0.001f;
	vec3 pos = worldPosition.xyz;
	pos.y = texture(height_map, pos.xz / terrain_size).x * maxHeight + bias;

	for(int i = 0; i < iters; i++)
	{
		pos += sunDir * step;

		// if our ray is below the ground, we have a hit
		// normalize the position by the terrain size to get UVs
		if (pos.y < texture(height_map, pos.xz / terrain_size).x * maxHeight)
		{
			color *= .5f;
			break;
		}
		// stop marching if we're above max height
		if (pos.y > maxHeight)
		{
			break;
		}
	}

	FragColor = vec4(color, 1.f);
}