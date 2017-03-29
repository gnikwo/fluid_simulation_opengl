#version 430 core

in vec2 in_Vertex;

out vec2 pos;

void main()
{

    gl_Position = vec4(in_Vertex, 0.0, 1.0);

    pos = in_Vertex;

}
