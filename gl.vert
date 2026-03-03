#version 450 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 textCoord;
out vec2 outCoord;

void main(void)
{
    gl_Position = vec4(pos,1.0);
    outCoord = textCoord;
}
