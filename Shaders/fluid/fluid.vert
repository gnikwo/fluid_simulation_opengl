#version 430 core

layout(location = 0) in vec2 in_Vertex;

void main()
{

    gl_Position = vec4(in_Vertex, 0.0, 1.0);

}
