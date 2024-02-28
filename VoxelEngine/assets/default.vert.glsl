#version 450 core

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_TexCoord;
layout(location = 3) in vec3 i_PositionOffset;

uniform mat4 u_ViewProjection;

void main() {
    vec3 pos = i_Position + i_PositionOffset;
    gl_Position = u_ViewProjection * vec4(pos, 1.0);
}
