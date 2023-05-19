#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec4 inFragmentNormal[];

const float MAGNITUDE = 0.2;

void main() {    
    gl_Position = gl_in[0].gl_Position;
    EmitVertex(); 
    gl_Position = gl_in[0].gl_Position + inFragmentNormal[0] * MAGNITUDE;
    EmitVertex(); 
    EndPrimitive();
    
    gl_Position = gl_in[1].gl_Position;
    EmitVertex(); 
    gl_Position = gl_in[1].gl_Position + inFragmentNormal[1] * MAGNITUDE;
    EmitVertex(); 
    EndPrimitive();
    
    gl_Position = gl_in[2].gl_Position;
    EmitVertex(); 
    gl_Position = gl_in[2].gl_Position + inFragmentNormal[2] * MAGNITUDE;
    EmitVertex(); 

    EndPrimitive();
}