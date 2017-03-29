#version 430 core

uniform sampler3D fluid;

in vec2 pos;

out vec4 out_color;

void main()
{

    out_color = vec4(texture(fluid, vec3(pos, 0.0)).xyz, 1.0);

}
