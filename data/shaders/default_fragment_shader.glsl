#version 460 core

out vec4 FragColor;

in vec3 norm;
in vec3 position;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	vec3 light_source = vec3(10.f, 2.f, 10.f);

	float cos_theta = abs(dot(normalize(light_source) - normalize(position), norm));

	//FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), .2);
	FragColor = vec4(1.f, 0.f, 0.f, 1.f) * cos_theta;
}