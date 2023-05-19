#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 inFragmentPos[];
in vec3 inFragmentNormal[];
in vec2 inFragmentTexCoord[];

out vec3 fragmentPos;
out vec3 fragmentNormal;
out vec2 fragmentTexCoord;

uniform float time;

vec3 get_normal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 pos, vec3 normal)
{
    float strength = 2.0;
    vec3 dir = normal * ((sin(time) + 1) / 2) * strength;
    return pos + vec4(dir, 1.0);
}

void main() {    
    vec3 normal = (inFragmentNormal[0] + inFragmentNormal[1] + inFragmentNormal[2]) / 3;//get_normal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    fragmentPos = inFragmentPos[0];
    fragmentNormal = inFragmentNormal[0];
    fragmentTexCoord = inFragmentTexCoord[0];
    EmitVertex(); 
    
    gl_Position = explode(gl_in[1].gl_Position, normal);
    fragmentPos = inFragmentPos[1];
    fragmentNormal = inFragmentNormal[1];
    fragmentTexCoord = inFragmentTexCoord[1];
    EmitVertex(); 
    
    gl_Position = explode(gl_in[2].gl_Position, normal);
    fragmentPos = inFragmentPos[2];
    fragmentNormal = inFragmentNormal[2];
    fragmentTexCoord = inFragmentTexCoord[2];
    EmitVertex(); 

    EndPrimitive();
}