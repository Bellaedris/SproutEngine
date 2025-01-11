#version 460

in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D frame;
uniform float exposure;
uniform float gamma;

void main() {
    vec3 hdrColor = texture(frame, texcoord).xyz;

    //reinhard tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction
    mapped = pow(mapped, vec3(1.f / gamma));

    FragColor = vec4(mapped, 1.f);
}
