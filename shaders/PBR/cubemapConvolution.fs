#version 460

#define M_PI 3.1415926535897932384626433832795

in vec3 pos;

out vec4 FragColor;

uniform samplerCube hdrMap;

void main() {
    vec3 normal = normalize(pos); // normal on a sphere is the normalized position

    vec3 up = vec3(0.f, 1.f, 0.f);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    vec3 irradiance = vec3(0);
    float step = .025f;
    float samples = .0f;
    for(float phi = .0f; phi < 2.f * M_PI; phi += step)
    for(float theta = .0f; theta < M_PI * .5f; theta += step)
    {
        // from the 2 angles, get coordinates in world space
        vec3 sphericalToWorld = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        // transform the coordinates in world space, oriented around normal
        vec3 hemisphereCoords = sphericalToWorld.x * right + sphericalToWorld.y * up + sphericalToWorld.z * normal;

        irradiance += texture(hdrMap, hemisphereCoords).xyz * cos(theta) * sin(theta);
        samples++;
    }
    irradiance = irradiance * M_PI / float(samples);

    FragColor = vec4(irradiance, 1.f);
}
