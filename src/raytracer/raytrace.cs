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
    vec4 ambiant;
    vec4 diffuse;
    vec4 specular;
    vec4 dir;
};

layout(std140, binding = 0) uniform Lights
{
    DirectionalLight lights[50];
};

#define LIGHT_NUMBER 2
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

vec3 rayColor(in Ray r, in Scene scene)
{
    vec3 color = vec3(0);
    HitInfo hit;

    if (hitScene(scene, r, 0.0001, 1000000, hit))
    {
        //color = vec3(hit.normal);
        for(int i = 0; i < LIGHT_NUMBER; i++)
        {
            //bounce a ray towards the light to check shadows
            Ray visibility = Ray(hit.intersection + hit.normal * 0.0001, -scene.lights[i].dir.xyz - hit.intersection);
            HitInfo hitVisibility;
            if (!hitScene(scene, visibility, 0.0001, 1000000, hitVisibility))
            {
                float cosTheta = max(dot(hit.normal, -scene.lights[i].dir.xyz), 0);

                color += scene.materials[hit.matIndex].ambiant + scene.materials[hit.matIndex].diffuse * scene.lights[i].diffuse.xyz * cosTheta;
            }
        }
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
    m2.ambiant = vec3(.0, 0.05, .0);
    m2.diffuse = vec3(.9, .9, .9);

    scene.spheres[0] = s1;
    scene.spheres[1] = s2;

    DirectionalLight light = DirectionalLight(vec4(0, 0, 0, 0), vec4(.9, 0, 0, 0), vec4(0, 0, 0, 0), normalize(vec4(1, -1, -1, 0)));
    scene.lights[0] = light;

    DirectionalLight light2 = DirectionalLight(vec4(0, 0, 0, 0), vec4(0, 0, .9, 1.f), vec4(0, 0, 0, 1), normalize(vec4(-1, -1, -1, 0.f)));
    scene.lights[1] = light2;

    scene.materials[0] = m1;
    scene.materials[1] = m2;

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

    int samples = 10;
    vec3 color = vec3(0);
    for(int k = 0; k < samples; k++)
    {
        float offsetx = random(vec2(x + k, y + k));
        float offsety = random(vec2(offsetx + k, y + k));

        float u = float(x + offsetx) / float(imageDim.x);
        float v = float(y + offsety) / float(imageDim.y);
        Ray r = Ray(cameraPos, viewportUpperLeft + (float(x) + offsetx) * deltaU + (float(y) + offsety) * deltaV - cameraPos);

        color += rayColor(r, scene);
    }
    color /= samples;

    vec4 finalColor = vec4(color, 1);

    imageStore(imageOutput, current, finalColor);
}
