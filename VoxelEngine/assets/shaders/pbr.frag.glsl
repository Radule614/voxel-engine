#version 450 core

layout (location = 0) out vec4 o_Color;

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
    vec4 Diffuse;
};

uniform DirectionalLight u_DirectionalLight;
uniform vec3 u_CameraPos;

uniform bool u_HasBaseTexture;
uniform vec4 u_BaseColorFactor;
uniform sampler2D u_BaseTexture;

vec3 CalculateDirectionalLight(DirectionalLight light, Material material, vec3 normal, vec3 viewDir) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.Direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 128);

    vec3 ambient = light.Ambient * 0.3 * vec3(material.Diffuse);
    vec3 diffuse = light.Diffuse * diff * vec3(material.Diffuse);

    return (ambient + diffuse);
}

Material CreateMaterial() {
    Material material;

    if (u_HasBaseTexture) {
        material.Diffuse = texture(u_BaseTexture, i_Fragment.FragTexCoords);
    } else {
        material.Diffuse = u_BaseColorFactor;
    }

    return material;
}

void main() {
    Material material = CreateMaterial();

    if (material.Diffuse.a < 0.5) {
        discard;
    }

    vec3 viewDir = normalize(u_CameraPos - i_Fragment.FragPos);
    o_Color = vec4(CalculateDirectionalLight(u_DirectionalLight, material, i_Fragment.FragNormal, viewDir), 1.0);
}
