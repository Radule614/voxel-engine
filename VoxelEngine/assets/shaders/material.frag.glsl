#version 450 core

#define NR_POINT_LIGHTS 8
#define NR_SPOT_LIGHTS 4

in o_Vertex {
	vec3 FragNormal;
    vec3 FragPos;
    vec2 FragTexCoords;
    mat3 TBN;
} i_Fragment;

out vec4 o_Fragment;

struct Material {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};

struct PointLight {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Constant;
    float Linear;
    float Quadratic;
};

struct DirectionalLight {
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float CutOff;
    float OuterCutOff;
    float Constant;
    float Linear;
    float Quadratic;
};

uniform PointLight u_PointLights[NR_POINT_LIGHTS];
uniform DirectionalLight u_DirectionalLight;
uniform SpotLight u_SpotLights[NR_SPOT_LIGHTS];
uniform vec3 u_CameraPos;

uniform sampler2D u_TextureDiffuse_1;
uniform sampler2D u_TextureDiffuse_2;
uniform sampler2D u_TextureDiffuse_3;
uniform sampler2D u_TextureSpecular_1;
uniform sampler2D u_TextureSpecular_2;
uniform sampler2D u_TextureNormal_1;
uniform sampler2D u_TextureNormal_2;

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
vec3 CalculatePointlight(PointLight light, Material mat, vec3 fragPos, vec3 normal, vec3 viewDir) {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.Position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), mat.Shininess);

    vec3 ambient = light.Ambient * mat.Ambient;
    vec3 diffuse = light.Diffuse * diff * mat.Diffuse;
    vec3 specular = light.Specular * spec * mat.Specular;

    float distance = length(light.Position - fragPos);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}
vec3 CalculateSpotLight(SpotLight light, Material mat, vec3 fragPos, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(light.Position - fragPos);
    float theta = dot(lightDir, normalize(-light.Direction));
    float epsilon = light.CutOff - light.OuterCutOff;
    float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

    if(theta > light.OuterCutOff) {
        vec3 norm = normalize(normal);
        float diff = max(dot(norm, lightDir), 0.0);
    
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), mat.Shininess);

        vec3 ambient = light.Ambient * mat.Ambient;
        vec3 diffuse = light.Diffuse * diff * mat.Diffuse;
        vec3 specular = light.Specular * spec * mat.Specular;

        float distance = length(light.Position - fragPos);
        float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));

        ambient *= attenuation;
        diffuse *= attenuation * intensity;
        specular *= attenuation * intensity;
    
        return (ambient + diffuse + specular);
    }
    return vec3(0);
}

void main() {
    vec3 viewDir = normalize(u_CameraPos - i_Fragment.FragPos);
    Material mat;
    mat.Diffuse = vec3(texture(u_TextureDiffuse_1, i_Fragment.FragTexCoords));
    mat.Ambient = 0.005 * mat.Diffuse;
    mat.Specular = vec3(texture(u_TextureSpecular_1, i_Fragment.FragTexCoords));
    mat.Shininess = 128.0;

    vec3 normal = vec3(texture(u_TextureNormal_1, i_Fragment.FragTexCoords));
    normal = normal * 2.0 - 1.0;  
    normal = normalize(i_Fragment.TBN * normal); 

    vec3 color = CalculateDirectionalLight(u_DirectionalLight, mat, normal, viewDir);
    /*
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        color += CalculatePointlight(u_PointLights[i], mat, i_Fragment.FragPos, i_Fragment.FragNormal, viewDir);
    for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        color += CalculateSpotLight(u_SpotLights[i], mat, i_Fragment.FragPos, i_Fragment.FragNormal, viewDir);
    */
    float gamma = 2.2;
    o_Fragment = vec4(pow(color, vec3(1.0/gamma)), 1.0);
}