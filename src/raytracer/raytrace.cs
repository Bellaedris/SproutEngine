#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imageOutput;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Sphere
{
    vec3 center;
    float radius;
};

bool intersectSphere(Ray r, Sphere s)
{
    return true;
}

void main() {

//    vec3 center = vec3(0, 0, 0); //camera pos
//
//    float focalLength = 1.f;
//    float viewportHeight = 2.0;
//    float viewportWidth = viewportHeight * (float(gl_NumWorkGroups.x) / gl_NumWorkGroups.y);
//
//    vec3 viewportU = vec3(gl_NumWorkGroups.x, 0, 0);
//    vec3 viewportV = vec3(0, -gl_NumWorkGroups.y, 0);
//
//    vec3 deltaU = viewportU / float(gl_NumWorkGroups.x);
//    vec3 deltaV = viewportV / float(gl_NumWorkGroups.y);
//
//    vec3 viewportUpperLeft = center - vec3(0, 0, focalLength) - viewportU / 2.f - viewportV / 2.f;
//    vec3 pixelOrigin = viewportUpperLeft - 0.5f * (deltaU + deltaV);

    vec4 val = vec4(.0, .0, .0, 1.);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    val.x = float(texelCoord.x) / gl_NumWorkGroups.x;
    val.y = float(texelCoord.y) / gl_NumWorkGroups.y;

//    Ray r;
//    r.origin = center;
//    vec3 currentSample = pixelOrigin + float(texelCoord.x) * deltaU + float(texelCoord.y) * deltaV;
//    r.direction = normalize(currentSample - center);

    imageStore(imageOutput, texelCoord, val);
}
