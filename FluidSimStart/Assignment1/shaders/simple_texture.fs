#version 430

in vec2 texcoord;
out vec4 out_color;

uniform sampler2D tex;

void main()
{
    vec4 diff_tex = texture(tex, texcoord);
    out_color = diff_tex;
}


