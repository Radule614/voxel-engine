#version 450 core

layout(location = 0) out vec4 o_Color;

in o_Vertex {
    vec3 FragNormal;
    vec3 FragPos;
    vec2 FragTexCoords;
} i_Fragment;

uniform sampler2D u_Atlas;

void main() {
    vec4 texColor = texture(u_Atlas, i_Fragment.FragTexCoords);
    if (texColor.a < 0.1)
        discard;

    vec3 lightColor = vec3(1);
    vec3 lightDir = normalize(vec3(1, 1.7, 1));

    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(lightDir, i_Fragment.FragNormal), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * vec3(texColor);
    o_Color = vec4(result, texColor.w);
}
