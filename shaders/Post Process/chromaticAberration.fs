#version 460 core

in vec2 texcoord;

uniform sampler2D framebuffer;

uniform float strength;
uniform int fadeFromCenter;

out vec4 FragColor;

void main()
{
	float redOffset   =  0.009;
	float greenOffset =  0.006;
	float blueOffset  = -0.006;

	vec2 size = textureSize(framebuffer, 0).xy;

	//float dist_from_center = fadeFromCenter == 0 ? 1 : distance(gl_FragCoord.xy / size, vec2(.5, .5));

	vec2 direction = texcoord - vec2(.5, .5);

	float r = texture(framebuffer, texcoord + vec2(redOffset) * direction).r;
	float g = texture(framebuffer, texcoord + vec2(greenOffset) * direction).g;
	float b = texture(framebuffer, texcoord + vec2(blueOffset) * direction).b;

	FragColor = vec4(r, g, b, 1.);
}