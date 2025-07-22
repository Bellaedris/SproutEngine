#version 460

in vec3 texCoords;

out vec4 FragColor;

uniform vec3 sunColor;
uniform vec3 sunDir;
uniform float sunSize;

void main() {
    // fake sun
    float sunFactor = dot(normalize(sunDir), normalize(-texCoords));
    float sunMask = (1.f - step(sunFactor, sunSize));
    vec3 sun = sunMask * sunColor;

    vec3 skyColor = vec3(0.529, 0.808, 0.98);
    vec3 groundColor = vec3(0.816, 0.792, 0.573);
    vec3 horizonColor = vec3(.8, .8, .8);
    // sky part
    float skyFactor = clamp(texCoords.y, .0f, 1.f);
    float skyPowered = pow(skyFactor, .5f);
    skyColor = skyPowered * skyColor;

    // ground part
    float groundFactor = -clamp(texCoords.y, -1.f, .0f);
    float groundPowered = pow(groundFactor, .5f);
    groundColor = groundPowered * groundColor;

    // horizon part
    float horizonPart = 1.f - (skyFactor + groundFactor);
    float horizonPowered = pow(horizonPart, 4.f);
    horizonColor = horizonColor * horizonPowered;

    vec3 background = skyColor + groundColor + horizonColor;

    vec3 color = mix(background, sun, sunMask);

    FragColor = vec4(color, 1.f);
}