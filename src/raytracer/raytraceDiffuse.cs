#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imageOutput;
layout(rgba32f, binding = 1) uniform image2D sphericalCoords;

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
//! intersection point
    vec3 intersection;
//! normal at the intersection point
    vec3 normal;
//! distance at which the intersection happened
    float distance;
//! true if front, false otherwise
    bool frontFace;
//! id of the material used by the primitive
    int matIndex;
};

struct Material
{
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
};

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

#define SPHERE_NUMBER 2

struct DirectionalLight
{
    vec3 dir;
    vec3 color;
};

#define LIGHT_NUMBER 1
#define MATERIAL_NUMBER 2

struct Scene
{
    Sphere spheres[SPHERE_NUMBER];
    DirectionalLight lights[LIGHT_NUMBER];
    Material materials[MATERIAL_NUMBER];
};

bool hitScene(in Scene scene, in Ray r, float hmin, float hmax, inout HitInfo hit)
{
    HitInfo current;
    float closest = hmax;
    bool hasHit = false;

    for(int i = 0; i < SPHERE_NUMBER; i++)
    {
        if (intersectSphere(r, scene.spheres[i], hmin, closest, current))
        {
            hit = current;
            closest = hit.distance;
            hasHit = true;
        }
    }
    return hasHit;
}

vec3 rayColor(in Ray r, in Scene scene, float u, float v)
{
    Ray tmp = r;
    HitInfo hit;
    vec3 color = vec3(0);
    int depth = 0;
    float modifier = 1.f;

    while(depth < 10)
    {
        if (hitScene(scene, r, 0.0001, 1000000, hit))
        {
            // Generate random offset using the depth and current coordinates for randomness
            float offsetX = random(vec2(u + depth * 10.0, v * 1000.0 + depth));
            float offsetY = random(vec2(u * 1000.0 + depth * 10.0, v + depth * 100.0));
            ivec2 offset = ivec2(gl_GlobalInvocationID.xy) + ivec2(offsetX * 100.0, offsetY * 100.0);
            offset = clamp(offset, ivec2(0), ivec2(imageSize(sphericalCoords)) - ivec2(1));

            vec3 randDir = hit.normal + imageLoad(sphericalCoords, offset).rgb;
            if(dot(randDir, hit.normal) < 0.f)
                return -randDir;
            r = Ray(hit.intersection + hit.normal * 0.0001, randDir - hit.intersection);
            float cosTheta = max(dot(hit.normal, -scene.lights[0].dir), 0);

            modifier++;
        }
        else
        {
            vec3 unitDir = normalize(r.direction);
            float y = (unitDir.y + 1.f) * .5f;

            color = mix(vec3(1.f, 1.f, 1.f), vec3(.5f, .7f, 1.f), y);
            return pow(2, -modifier) * color;
        }
        depth++;
    }

    return color;
}

// vec3 rayColor(in Ray r, in Scene scene)
//{
//    vec3 color;
//    HitInfo hit;
//
//    if (hitScene(scene, r, 0.0001, 1000000, hit))
//    {
//        //color = vec3(hit.normal);
//
//        float cosTheta = max(dot(hit.normal, -scene.lights[0].dir), 0);
//
//        color = scene.materials[hit.matIndex].ambiant + scene.materials[hit.matIndex].diffuse * scene.lights[0].color * cosTheta;
//    }
//    else
//    {
//        vec3 unitDir = normalize(r.direction);
//        float y = (unitDir.y + 1.f) * .5f;
//
//        color = mix(vec3(1.f, 1.f, 1.f), vec3(.5f, .7f, 1.f), y);
//    }
//
//    return color;
//}

void main()
{
    //scene definition
    Scene scene;
    Sphere s1;
    s1.center = vec3(0, 0, -1);
    s1.radius = .5;
    s1.matIndex = 0;

    Sphere s2;
    s2.center = vec3(0, -100.5, -1);
    s2.radius = 100;
    s2.matIndex = 1;

    Material m1;
    m1.ambiant = vec3(.05, .0, .0);
    m1.diffuse = vec3(.9, .9, .9);

    Material m2;
    m2.ambiant = vec3(.0, .05, .0);
    m2.diffuse = vec3(.9, .4, .9);

    scene.spheres[0] = s1;
    scene.spheres[1] = s2;

    DirectionalLight light = DirectionalLight(normalize(vec3(1, -1, -1)), vec3(1, 1, 1));
    scene.lights[0] = light;

    scene.materials[0] = m1;
    scene.materials[1] = m2;

    //sending the rays
    uvec2 imageDim = gl_NumWorkGroups.xy;
    ivec2 current = ivec2(gl_GlobalInvocationID.xy);
    int x = current.x;
    int y = current.y;

    float focalLength = 1.f;
    vec3 center = vec3(0., 0., 0.); // camera position
    vec3 horizontal = vec3(3.55555555, 0., 0.);
    vec3 vertical = vec3(0., 2., 0.);
    vec3 viewportUpperLeft = center - vec3(0, 0, focalLength) - horizontal / 2.f - vertical / 2.f;

    int samples = 10;
    vec3 color = vec3(0);
    for(int k = 0; k < samples; k++)
    {
        float offsetx = random(vec2(x + k, y + k));
        float offsety = random(vec2(offsetx + k, y + k));

        float u = float(current.x + offsetx) / float(imageDim.x);
        float v = float(current.y + offsety) / float(imageDim.y);
        Ray r = Ray(center, viewportUpperLeft + u * horizontal + v * vertical - center);

        color += rayColor(r, scene, u, v);
    }
    color /= samples;

    vec4 finalColor = vec4(color, 1);

    //    vec4 val = vec4(.0, .0, .0, 1.);
    //    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    //
    //    val.x = float(texelCoord.x) / gl_NumWorkGroups.x;
    //    val.y = float(texelCoord.y) / gl_NumWorkGroups.y;

    imageStore(imageOutput, current, finalColor);
    //imageStore(imageOutput, current, imageLoad(sphericalCoords, current));
}
