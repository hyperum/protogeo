#version 330 core

layout(location = 0)in float position;
layout(location = 1)in vec3 color;

uniform mat4 mvp_transformation;

out VS_OUT {
    vec3 color;
} vs_out;

void main()
{
    vs_out.color = color;
    gl_Position = mvp_transformation * vec4(position, position, 0.0, 1.0);
}