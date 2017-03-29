#version 430

layout(points )in;
layout(points ,max_vertices = 1 )out;

uniform int layer;

void main(){

    gl_Position=gl_in[0].gl_Position; //or vec4(0.0,0.0,0.0,1.0)

    gl_Layer = layer;

    EmitVertex();
    EndPrimitive();

}
