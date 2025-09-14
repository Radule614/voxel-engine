#version 450 core

layout (location = 0) out vec4 o_Color;

in o_Vertex {
    flat float Radiance;
    vec2 FragTexCoords;
} i_Fragment;

uniform sampler2D u_Atlas;

void main() {
    vec4 texColor = texture(u_Atlas, i_Fragment.FragTexCoords);

    if (texColor.a < 0.5) discard;

    float radiance = i_Fragment.Radiance < 0 ? 0.2 : i_Fragment.Radiance;

    vec4 color = vec4(texColor.xyz * radiance, texColor.a);

    o_Color = color;
}
