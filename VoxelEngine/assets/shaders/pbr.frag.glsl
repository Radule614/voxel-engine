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

uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_CameraPos;
uniform sampler2D u_TextureDiffuse_1;
uniform sampler2D u_TextureSpecular_1;
uniform vec3 u_Color;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.Direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 128);

    vec3 ambient = light.Ambient * 0.1 * u_Color;
    vec3 diffuse = light.Diffuse * diff * u_Color;

    return (ambient + diffuse);
}

void main() {
    vec3 viewDir = normalize(u_CameraPos - i_Fragment.FragPos);


    o_Color = vec4(CalculateDirectionalLight(u_DirectionalLight, i_Fragment.FragNormal, viewDir), 1.0);
}
