#version 330 core

layout (points) in;

layout (triangle_strip, max_vertices = 4) out;

out vec3 fColor;

uniform float dx;
uniform vec2 world_quadrant;
uniform bool is_triangle;

void main()
{
	if (gl_in[0].gl_Position.x * gl_in[0].gl_Position.y > 0) fColor = vec3(0, 1, 1);
	else fColor = vec3(1, 0, 0);

	if (!is_triangle)
	{
	    gl_Position = gl_in[0].gl_Position; 
	    EmitVertex();   
	    gl_Position = gl_in[0].gl_Position + vec4(dx/world_quadrant.x, 0.0, 0.0, 0.0);
	    EmitVertex();
	    gl_Position = vec4(gl_in[0].gl_Position.x, 0.0, 0.0, 1.0);
	    EmitVertex();
	    gl_Position = gl_in[0].gl_Position + vec4(dx/world_quadrant.x,  -gl_in[0].gl_Position.y, 0.0, 0.0); //br
	    EmitVertex();
	}

    EndPrimitive();
}  