#version 450 core

layout(location = 0) out vec4 o_Color;

in o_Vertex {
    vec3 FragPos;
    uint Face;
    vec2 FragTexCoords;
} i_Fragment;

uniform sampler2D u_Atlas;

void main() {
    vec4 texColor = texture(u_Atlas, i_Fragment.FragTexCoords);
    if (texColor.a < 0.5)
        discard;
    o_Color = texColor;
}
