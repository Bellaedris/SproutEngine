#version 460

#define M_PI 3.1415926535897932384626433832795

in vec2 uv;

out vec2 FragColor;

float geometrySclickGGX(in float normDotDir, in float k)
{
    return normDotDir / (normDotDir * (1.f - k) + k);
}

float SmithGGX(in vec3 normal, in vec3 viewDir, in vec3 lightDir, in float roughness)
{
    // k is a remapping of roughness depending on if we're sampling ibl or direct lighting
    // here, IBL
    float a = roughness * roughness;
    float k = a / 2.f;

    return
    geometrySclickGGX(max(dot(normal, viewDir), 0), k)
    * geometrySclickGGX(max(dot(normal, lightDir), 0), k);
}

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

// Hammersley sequence (low discrepancy)
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
vec2 IntegrateBRDF(float NdotV, float roughness)
{
    vec3 V;
    V.x = sqrt(1.f - NdotV * NdotV);
    V.y = .0f;
    V.z = NdotV;

    float A = .0f;
    float B = .0f;

    vec3 N = vec3(.0f, .0f, 1.f);

    const uint sampleCount = 1024u;
    for(uint i = 0u; i < sampleCount; i++)
    {
        vec2 Xi = Hammersley(i, sampleCount);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.f * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);
        if(NdotL > .0f)
        {
            float G = SmithGGX(N, V, L, roughness);
            float G_vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.f - VdotH, 5.f);

            A += (1.f - Fc) * G_vis;
            B += Fc * G_vis;
        }
    }

    A /= float(sampleCount);
    B /= float(sampleCount);
    return vec2(A, B);
}

void main() {
    vec2 integratedBRDF = IntegrateBRDF(uv.x, uv.y);

    FragColor = integratedBRDF;
}
