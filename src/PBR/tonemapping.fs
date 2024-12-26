#version 460

in vec2 texcoord;

out vec4 FragColor;

uniform sampler2D frame;
uniform float exposure;

void main() {
    vec3 hdrColor = texture(frame, texcoord).xyz;

    //reinhard tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    FragColor = vec4(mapped, 1.f);
}
