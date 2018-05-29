#version 330 core

layout (points) in;

layout (triangle_strip, max_vertices = 6) out;

out vec3 fColor;

uniform float dy;
uniform float dx;
uniform vec2 world_quadrant;

void main()
{
	fColor = vec3(1, 0, 0);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();   
    gl_Position = gl_in[0].gl_Position + vec4(dx/world_quadrant.x, 0.0, 0.0, 0.0);// ur
    EmitVertex();

  	EndPrimitive();

	fColor = vec3(0, 0, 1);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();   
    gl_Position = gl_in[0].gl_Position + vec4(0.0, dy/world_quadrant.y, 0.0, 0.0);// ur
    EmitVertex();

 	EndPrimitive();

 	fColor = vec3(1, 0, 1);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();   
    gl_Position = gl_in[0].gl_Position + vec4(dx/world_quadrant.x, dy/world_quadrant.y, 0.0, 0.0);// ur
    EmitVertex();

	EndPrimitive();
}  