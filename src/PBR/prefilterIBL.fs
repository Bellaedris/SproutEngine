#version 460

#define M_PI 3.1415926535897932384626433832795

in vec3 pos;

out vec4 FragColor;

uniform samplerCube hdrMap;
uniform float roughness;

// Hammersley sequence (low discrepancy sequence)
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

// combine GGX's NDF and a mapping from polar to tangent
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;

    float phi = 2.0 * M_PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void main() {
    vec3 normal = normalize(pos);
    // as done in the Epic Game PBR, we consider that view = sample dir
    vec3 viewDir = normal;

    const uint sampleCount = 1024;
    float totalWeight = .0f;
    vec3 prefilteredColor = vec3(.0f);
    for(uint i = 0; i < sampleCount; i++)
    {
        vec2 Xi = Hammersley(i, sampleCount);
        vec3 H = ImportanceSampleGGX(Xi, normal, roughness);
        vec3 L = normalize(2.f * dot(viewDir, H) * H - viewDir);

        float NdotL = max(dot(normal, L), .0f);
        if(NdotL > .0f)
        {
            // we can reduce brigh dots caused by the low amount of samples using https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/
            // By instead of directly sampling the hdr map, sample a mip lvl of the map based on the PDF and roughness
            prefilteredColor += texture(hdrMap, L).xyz * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor /= totalWeight;

    FragColor = vec4(prefilteredColor, 1.f);
}
