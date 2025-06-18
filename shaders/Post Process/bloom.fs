#version 460 core

in vec2 texcoord;

uniform sampler2D framebuffer;
uniform sampler2D blurredBright;

out vec4 FragColor;

void main() {
    FragColor = texture(framebuffer, texcoord) + texture(blurredBright, texcoord);
}
