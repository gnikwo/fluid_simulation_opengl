#version 430 core

layout(triangles)in;
layout(triangle_strip ,max_vertices = 3 )out;

uniform int layer;

out vec4 pos;

void main(){

    for (int i = 0; i < 3; i++) {

        gl_Position=gl_in[i].gl_Position;
        gl_Layer = layer;

        pos = gl_in[i].gl_Position;

        EmitVertex();

    }

    EndPrimitive();

}
