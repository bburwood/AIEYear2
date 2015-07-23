#version 430

layout(location=0)in vec3 vertex_position;
layout(location=1)in vec2 vertex_texcoord;
layout(location=2)in vec3 vertex_normal;

uniform mat4 model;
uniform mat4 view_proj;

out vec2 texcoord;
out vec3 normal;
out vec3 world_pos;

void main()
{
    mat4 model_view_projection = view_proj * model;

    normal = vertex_normal;
    texcoord = vertex_texcoord;

    world_pos = (model * vec4(vertex_position, 1)).xyz;

    gl_Position = model_view_projection * vec4(vertex_position, 1);
}
