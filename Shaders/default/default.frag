#version 430

uniform sample3D fluid;

layout(location=0) out vec4 out_color; //GL_COLOR_ATTACHMENT0

void main()
{

    out_color = vec4(1.0,0.0,0.0,1.0);

}
