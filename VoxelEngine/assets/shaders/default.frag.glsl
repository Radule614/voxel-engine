#version 450 core

layout(location = 0) out vec4 o_Color;

in o_Vertex {
    vec3 FragNormal;
    vec3 FragPos;
    vec2 FragTexCoords;
} i_Fragment;

uniform sampler2D u_Atlas;

void main() {
    o_Color = texture(u_Atlas, i_Fragment.FragTexCoords);
}
