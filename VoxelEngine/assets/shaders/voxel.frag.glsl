#version 450 core

layout(location = 0) out vec4 o_Color;

in o_Vertex {
    vec3 FragNormal;
    vec3 FragPos;
    vec2 FragTexCoords;
} i_Fragment;

struct DirectionalLight {
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform sampler2D u_Atlas;
uniform DirectionalLight directionalLight;

vec3 calculateDirectionalLight(DirectionalLight light, vec3 diffuseColor, vec3 normal) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.Direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 ambient = light.Ambient * diffuseColor;
    vec3 diffuse = light.Diffuse * diff * diffuseColor;
    vec3 specular = light.Specular * diff * diffuseColor;
    return (ambient + diffuse + specular);
}

void main() {
    vec4 texColor = texture(u_Atlas, i_Fragment.FragTexCoords);
    if (texColor.a < 0.5)
        discard;
    o_Color = vec4(calculateDirectionalLight(directionalLight, vec3(texColor), i_Fragment.FragNormal), texColor.w);
}
