#version 460 core

in vec2 texcoord;

uniform sampler2D framebuffer;

out vec4 FragColor;

void main()
{
	vec2 offsetR = vec2(.001, .002);
	vec2 offsetG = vec2(-.002, .004);
	vec2 offsetB = vec2(.004, -.002);

	float dist_from_center = distance(gl_FragCoord.xy / vec2(1366, 768), vec2(.5, .5));

	float r = texture(framebuffer, texcoord + offsetR * dist_from_center).r;
	float g = texture(framebuffer, texcoord + offsetG * dist_from_center).g;
	float b = texture(framebuffer, texcoord + offsetB * dist_from_center).b;

	FragColor = vec4(r, g, b, 1.);
}