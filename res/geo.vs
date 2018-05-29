#version 330 core

in vec2 position;
//in vec3 color;
out vec3 Color;

uniform mat4 mvp_transformation;

void main()
{
    Color = vec3(0.0, 1.0, 0.0);
    gl_Position = mvp_transformation * vec4(position, 0.0, 1.0);
}