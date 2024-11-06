#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imageOutput;
layout(rgba32f, binding = 1) uniform image2D sphericalCoords;

uniform int spp; // samples per pixel
uniform int spb; // samples per bounces
uniform float emissiveIntensity;

// utilities
float random (vec2 st) {
    return fract(sin(dot(st.xy,
    vec2(12.9898,78.233)))*
    43758.5453123);
}

struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec3 rayAt(Ray r, float t)
{
    return r.origin + r.direction * t;
}

struct HitInfo
{
    vec3 intersection;
    vec3 normal;
    float distance;
    bool frontFace;
    int matIndex;
};

struct Material
{
    vec4 diffuse;
    vec4 emissive;
};

layout(binding = 4) uniform materialsData
{
    Material materials[100];
};

struct Triangle
{
    vec4 a;
    vec4 b;
    vec4 c;
    vec4 na;
    vec4 nb;
    vec4 nc;
    int matIndex;
    int fillA;
    int fillB;
    int fillC;
};

layout(std140, binding = 2) uniform triangles
{
    Triangle data[100];
};

uniform int numberOfTriangles;

bool intersectTriangle(in Ray r, in Triangle tri, float tmin, float tmax, inout HitInfo hit)
{
    float epsilon = 0.0001f;

    vec3 edge1 = tri.b.xyz - tri.a.xyz;
    vec3 edge2 = tri.c.xyz - tri.a.xyz;
    vec3 ray_cross_e2 = cross(r.direction, edge2);
    float det = dot(edge1, ray_cross_e2);

    if (det > -epsilon && det < epsilon)
    return false;    // This ray is parallel to this triangle.

    float inv_det = 1.0 / det;
    vec3 s = r.origin - tri.a.xyz;
    float u = inv_det * dot(s, ray_cross_e2);

    if (u < 0 || u > 1)
    return false;

    vec3 s_cross_e1 = cross(s, edge1);
    float v = inv_det * dot(r.direction, s_cross_e1);

    if (v < 0 || u + v > 1)
    return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = inv_det * dot(edge2, s_cross_e1);

    if (t > tmin && t < tmax) // ray intersection
    {
        hit.intersection = r.origin + r.direction * t;
        hit.distance = t;
        hit.matIndex = tri.matIndex;
        float w = 1 - u - v;
        vec3 norm = u * tri.na.xyz + v * tri.nb.xyz + w * tri.nc.xyz / 3.f;

        if (dot(norm, r.direction) > 0.f) {
            hit.normal = -norm;
            hit.frontFace = false;
        }
        // same side, we are outside
        else
        {
            hit.normal = norm;
            hit.frontFace = true;
        }
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
    return false;
}

struct Sphere
{
    vec3 center;
    float radius;
    int matIndex;
};

bool intersectSphere(in Ray r, in Sphere s, float tmin, float tmax, inout HitInfo hit)
{
    vec3 oc = s.center - r.origin;

    float a = dot(r.direction, r.direction);
    float h = dot(r.direction, oc);
    float c = dot(oc, oc) - s.radius * s.radius;

    float delta = h * h - a * c;
    if (delta < 0.f)
    return false;

    // find nearest result in range
    float sqrtDelta = sqrt(delta);
    float root = (h - sqrtDelta) / a;
    if (root < tmin || root > tmax)
    {
        root = (h + sqrtDelta) / a;
        if (root < tmin || root > tmax)
        return false;
    }

    hit.distance = root;
    hit.intersection = rayAt(r, root);
    hit.matIndex = s.matIndex;

    vec3 norm = (hit.intersection - s.center) / s.radius;
    // opposite side, we are inside the sphere
    if (dot(norm, r.direction) > 0.f) {
        hit.normal = -norm;
        hit.frontFace = false;
    }
    // same side, we are outside
    else
    {
        hit.normal = norm;
        hit.frontFace = true;
    }

    return true;
}

struct DirectionalLight
{
    vec4 ambiant;
    vec4 diffuse;
    vec4 specular;
    vec4 dir;
};

layout(std140, binding = 3) uniform Lights
{
    DirectionalLight lights[50];
};

uniform int numberOfLights;

bool hitScene(in Ray r, float hmin, float hmax, inout HitInfo hit)
{
    HitInfo current;
    float closest = hmax;
    bool hasHit = false;

    for(int i = 0; i < numberOfTriangles; i++)
    {
        if (intersectTriangle(r, data[i], hmin, closest, current))
        {
            hit = current;
            closest = hit.distance;
            hasHit = true;
        }
    }
    return hasHit;
}

vec3 rayColor(in Ray r, in int x, in int y)
{
    vec3 light = vec3(0);
    vec3 color = vec3(0);
    HitInfo hit;

    if (hitScene(r, 0.0001, 1000000, hit))
    {
        for(int i = 0; i < spb; i++)
        {
            //bounce a ray randomly in the normal hemisphere to check ambiant occlusion
            // Generate random offset using the current ray index and current coordinates for randomness
            ivec2 offset = ivec2(x + i * 6859, y + i * 6843);
            offset = ivec2(mod(offset, ivec2(imageSize(sphericalCoords) - ivec2(1))));

            vec3 randDir = imageLoad(sphericalCoords, offset).rgb;
            if(dot(randDir, hit.normal) < 0.f)
            randDir = -randDir;

            Ray visibility = Ray(hit.intersection + hit.normal * 0.0001, randDir);
            HitInfo hitVisibility;
            if (hitScene(visibility, 0.0001, 1000000, hitVisibility))
            {
                light += materials[hitVisibility.matIndex].emissive.xyz * emissiveIntensity;
            }
        }
        light /= spb;
        color = materials[hit.matIndex].diffuse.xyz * light + materials[hit.matIndex].emissive.xyz;
    }
    else
    {
        vec3 unitDir = normalize(r.direction);
        float y = (unitDir.y + 1.f) * .5f;

        color = mix(vec3(1.f, 1.f, 1.f), vec3(.5f, .7f, 1.f), y);
    }

    return color;
}

uniform float fov;
uniform float aspectRatio;
uniform vec3 cameraPos;
uniform vec3 camDir;
uniform vec3 camRight;
uniform vec3 camUp;

void main()
{
    //sending the rays
    uvec2 imageDim = gl_NumWorkGroups.xy;
    ivec2 current = ivec2(gl_GlobalInvocationID.xy);
    int x = current.x;
    int y = current.y;

    float focal = length(cameraPos - camDir);
    float h = tan(radians(fov) / 2.f);
    float height = 2.f * h * focal;
    float width = height * aspectRatio;

    vec3 viewportU = width * camRight;
    vec3 viewportV = height * camUp;

    vec3 deltaU = viewportU / float(imageDim.x);
    vec3 deltaV = viewportV / float(imageDim.y);

    vec3 viewportUpperLeft = cameraPos - focal * -camDir - viewportU / 2.f - viewportV / 2.f;

    vec3 color = vec3(0);
    for(int k = 0; k < spp; k++)
    {
        float offsetx = random(vec2(x + k, y + k));
        float offsety = random(vec2(offsetx + k, y + k));

        float u = float(x + offsetx) / float(imageDim.x);
        float v = float(y + offsety) / float(imageDim.y);
        Ray r = Ray(cameraPos, viewportUpperLeft + (float(x) + offsetx) * deltaU + (float(y) + offsety) * deltaV - cameraPos);

        color += rayColor(r, x, y);
    }
    color /= spp;

    vec4 finalColor = vec4(color, 1);

    imageStore(imageOutput, current, finalColor);
}
