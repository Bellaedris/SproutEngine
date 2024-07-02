#version 460 core

#define RAY_MAX = 10000.f;

in vec2 texcoord;
in vec3 cameraWorldPos;

uniform sampler2D framebuffer;

struct Ray
{
	vec3 o;
	vec3 d;
};

struct Hit
{
	float d;
	float u;
	float v;
};

struct Triangle
{
	vec3 a;
	vec3 e1;
	vec3 e2;
	vec3 na, nb, nc;
};
uniform Triangle triangles;
uniform int triangle_count;

bool triangle_intersect( const in Triangle triangle, const in Ray r, const in float htmax, out Hit h)
{
	vec3 tvec= r.o - triangle.a;
	vec3 qvec= cross(tvec, triangle.e1);
	vec3 pvec= cross(r.d, triangle.e2);

	float det= dot(triangle.e1, pvec);
	float inv_det= 1.0f / det;

	h.d = dot(triangle.e2, qvec) * inv_det;
	h.u = dot(tvec, pvec) * inv_det;
	h.v = dot(r.d, qvec) * inv_det;

	return all(greaterThan(vec3(h.u, h.v, h.d), vec3(0))) && all(lessThan(vec2(h.u+h.v, h.d), vec2(1, htmax)));
}

out vec4 FragColor;

void main()
{
	vec4 color = vec4(0, 0, 0, 1);

	Ray r;
	r.o = cameraWorldPos;
	r.d = vec3(uv.x, uv.y, 1);

	Hit closest;
	for(int i = 0; i < triangle_count; i++)
	{
		Hit h;
		if (triangle_intersect(triangles[i], r, 1.f, h))
		{
			color = vec4(1, 1, 1, 1);
		}
	}

	FragColor = vec4(color);
}