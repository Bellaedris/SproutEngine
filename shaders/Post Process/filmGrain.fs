#version 460 core

in vec2 texcoord;

uniform sampler2D framebuffer;

uniform float strength;

out vec4 FragColor;

// White noise, thanks https://www.shadertoy.com/view/tlcBRl
float noise1(float seed1,float seed2){
	return(
	fract(seed1+12.34567*
	fract(100.*(abs(seed1*0.91)+seed2+94.68)*
	fract((abs(seed2*0.41)+45.46)*
	fract((abs(seed2)+757.21)*
	fract(seed1*0.0171))))))
	* 1.0038 - 0.00185;
}

float noise2(float seed1,float seed2){
	float buff1 = abs(seed1+100.94) + 1000.;
	float buff2 = abs(seed2+100.73) + 1000.;
	buff1 = (buff1*fract(buff2*fract(buff1*fract(buff2*0.63))));
	buff2 = (buff2*fract(buff2*fract(buff1+buff2*fract(seed1*0.79))));
	buff1 = noise1(buff1, buff2);
	return(buff1 * 1.0038 - 0.00185);
}

void main()
{
	float whiteNoise = noise2(texcoord.x * 100.f, texcoord.y * 100.f);

	vec3 color = texture(framebuffer, texcoord).xyz;

	vec3 noisedColor = color + color * whiteNoise * strength;

	FragColor = vec4(noisedColor, 1.);
}