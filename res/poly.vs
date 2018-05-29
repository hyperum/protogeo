#version 330 core

layout(location = 0)in vec2 position;
layout(location = 1)in float radius;
layout(location = 2)in vec3 color;

uniform mat4 mvp_transformation;

out VS_OUT {
	float radius;
    vec3 color;
} vs_out;

void main()
{
    vs_out.color = color;
    vs_out.radius = radius;
    gl_Position = mvp_transformation * vec4(position, 0.0, 1.0);
}