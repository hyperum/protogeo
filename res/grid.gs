#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

//uniform vec2 viewport;

void main() {
	fColor = gs_in[0].color;

	gl_Position = gl_in[0].gl_Position + vec4(-10000, 0.0, 0.0, 0.0); 
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(10000, 0.0, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();

	gl_Position = gl_in[0].gl_Position + vec4(0.0, -10000, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.0, 10000, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
}  