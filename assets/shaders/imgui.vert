#version 460 core

layout(location=0) in vec2 position;
layout(location=1) in vec4 in_color;

out vec4 color;

uniform mat4 projection;

void main()
{
    color = in_color;
    gl_Position = projection * vec4(position, 0.0, 1.0);
}
