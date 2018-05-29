#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 65) out;

in VS_OUT {
	float radius;
    vec3 color;
} gs_in[];

out vec3 fColor;

uniform vec2 screen_to_world; //eg 6.4x3.2 rename: screen quadrant
uniform vec2 screen_to_pixel; //eg 1280x720 -> 640x320 rename: pixel quadrant

float min (float a, float b)
{
	return a < b? a : b;
}

float max (float a, float b)
{
	return a > b? a : b;
}

//screen space -1..1. we take the radius in world coordinates, divide by (offset_w_h/2) = quadrant_size.w_h to attain screen coords. offset = zoo
const float PI = 3.141592653582795028;

void main()
{
	fColor = gs_in[0].color;

	float radius_x_screen = gs_in[0].radius / screen_to_world.x; 
	float radius_y_screen = gs_in[0].radius / screen_to_world.y; 
	int vertices = int(min(64, PI * max(radius_x_screen * screen_to_pixel.x, radius_y_screen * screen_to_pixel.y)));

	for (int i = 0; i <= vertices; i++) 
	{
	    float ang = (PI * 2.0 / vertices) * i;
	    vec2 offset = vec2(cos(ang) * radius_x_screen, sin(ang) * radius_y_screen);

	    gl_Position = gl_in[0].gl_Position + vec4(offset, 0.0, 0.0);

	    EmitVertex();
	}
	EndPrimitive();
}  