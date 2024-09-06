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

struct Material {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};

uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_CameraPos;
uniform sampler2D u_TextureDiffuse_1;
uniform sampler2D u_TextureSpecular_1;

vec3 CalculateDirectionalLight(DirectionalLight light, Material mat, vec3 normal, vec3 viewDir) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.Direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), mat.Shininess);

    vec3 ambient = light.Ambient * mat.Ambient;
    vec3 diffuse = light.Diffuse * diff * mat.Diffuse;
    vec3 specular = light.Specular * spec * mat.Specular;

    return (ambient + diffuse + specular);
}

void main() {
    vec3 viewDir = normalize(u_CameraPos - i_Fragment.FragPos);
    Material mat;
    mat.Diffuse = vec3(texture(u_TextureDiffuse_1, i_Fragment.FragTexCoords));
    mat.Ambient = 0.03 * mat.Diffuse;
    mat.Specular = vec3(texture(u_TextureSpecular_1, i_Fragment.FragTexCoords));
    mat.Shininess = 128.0;
    o_Color = vec4(CalculateDirectionalLight(u_DirectionalLight, mat, i_Fragment.FragNormal, viewDir), 1.0);
}
