#version 460

in vec3 pos;

out vec4 FragColor;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(.1591f, .3183f);
vec2 SampleSphericalMap(in vec3 v)
{
    // map from spherical to cartesian coordinates
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += .5f;
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(pos));
    vec3 color = texture(equirectangularMap, uv).xyz;

    FragColor = vec4(color, 1.f);
}
