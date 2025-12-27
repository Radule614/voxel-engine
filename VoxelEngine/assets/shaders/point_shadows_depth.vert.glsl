#version 450 core

layout (location = 0) in vec3 i_Position;

uniform mat4 u_Model;

void main()
{
    gl_Position = u_Model * vec4(i_Position, 1.0);
}