#version 430 core

uniform sampler3D fluid;
uniform int init;

in vec4 pos;

layout(location=0) out vec4 out_color;

void main()
{

    vec4 color = texture(fluid, pos.xyz);

    //color.x += 0.1;

    if(init > 0)
        color.x = 0.9;

    //if(color.x > 1)
    //    color.x = 0;

    out_color = vec4(color.xyz, 1.0);

    //out_color = vec4(pos.xy, 0.0, 1.0);

}
