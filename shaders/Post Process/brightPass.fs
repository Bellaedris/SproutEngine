#version 460 core

in vec2 texcoord;

uniform sampler2D framebuffer;

uniform float threshold;

out vec4 FragColor;

void main() {
    vec3 inputColor = texture(framebuffer, texcoord).xyz;

    // convert the color to greyscale to compare to the threshold
    float brightness = dot(inputColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > threshold)
        FragColor = vec4(inputColor, 1.f);
    else
        FragColor = vec4(vec3(0.f), 1.f);
}
