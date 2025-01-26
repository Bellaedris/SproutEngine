#version 460

in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D frame;
uniform float exposure;
uniform float gamma;
uniform int technique;

vec3 reinhardt(vec3 hdrColor, float exposure)
{
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction needed
    return pow(mapped, vec3(1.f / gamma));
}

// from https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    vec3 mapped = clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.f, 1.f);
    // gamma correction needed
    return pow(mapped, vec3(1.f / gamma));
}

//from http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 filmic(vec3 hdrColor)
{
    hdrColor *= exposure;  // Exposure Adjustment
    vec3 x = max(vec3(0),hdrColor-0.004);
    vec3 retColor = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);
    return retColor;
}

void main() {
    vec3 hdrColor = texture(frame, texcoord).xyz;

    vec3 mapped;
    switch(technique)
    {
        case 0:
            mapped = reinhardt(hdrColor, exposure);
            break;
        case 1:
            mapped = ACESFilm(hdrColor);
            break;
        case 2:
            mapped = filmic(hdrColor);
            break;
    }

    FragColor = vec4(mapped, 1.f);
}
