#version 460 core

in vec2 texcoord;
in vec3 cameraToPixelRay;

uniform sampler2D framebuffer;
uniform sampler2D depthTexture;

uniform float zfar;
uniform float znear;
uniform vec3 cameraPos; // world space
uniform vec3 viewDir = vec3(0.f, 0.f, -1.f); // camera space
uniform float maxFogDistance;
uniform float fogDensity;
uniform vec4 fogColor;

out vec4 FragColor;

void main() {
    vec3 inputColor = texture(framebuffer, texcoord).xyz;
    float depth = texture(depthTexture, texcoord).x * 2.f - 1.f; // remap to -1;1

    // thanks https://cs.gmu.edu/~jchen/cs662/fog.pdf
    float p33 = (zfar + znear) / (znear - zfar);
    float p34 = (-2.f * zfar * znear) / (zfar - znear);

    float linearDepth = -p34 / (depth + p33);
    float fragDist = linearDepth / dot(cameraToPixelRay, viewDir);
    vec3 worldPos = fragDist * viewDir + cameraPos;

    float camToPixel = distance(worldPos, cameraPos);
    // linear factor
    //float fogFactor = (maxFog - camToPixel) / (maxFog - minFog);

    // exponential factor
    float fogFactor = 4.f * camToPixel / maxFogDistance;
    fogFactor = exp(-fogFactor * fogDensity * fogFactor * fogDensity);

    vec3 color = mix(fogColor.xyz, inputColor, fogFactor);

    FragColor = vec4(color, 1.f);
}
