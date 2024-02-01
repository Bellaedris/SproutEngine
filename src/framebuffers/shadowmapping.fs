#version 460 core

out vec4 FragColor;

// HANDLE NORMAL MATRIX WHEN APPLYING A UNIFORM SCALE
// take transpose of inverse of model applied to normal

void main()
{
	FragColor = vec4(1.f, 1.f, 1.f, 1.f);
}