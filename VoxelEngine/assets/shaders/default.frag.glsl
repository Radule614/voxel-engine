#version 330 core

#define NR_POINT_LIGHTS 8
#define NR_SPOT_LIGHTS 4

in o_Vertex {
	vec3 FragNormal;
    vec3 FragPos;
    vec2 FragTexCoords;
    mat3 TBN;
} FragmentIn;

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

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirectionalLight directionalLight;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform vec3 CameraPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;

vec3 calculateDirectionalLight(DirectionalLight light, Material mat, vec3 normal, vec3 viewDir) {
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
vec3 calculatePointlight(PointLight light, Material mat, vec3 fragPos, vec3 normal, vec3 viewDir) {
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
vec3 calculateSpotLight(SpotLight light, Material mat, vec3 fragPos, vec3 normal, vec3 viewDir) {
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
    vec3 viewDir = normalize(CameraPos - FragmentIn.FragPos);
    Material mat;
   
    mat.Diffuse = vec3(texture(texture_diffuse1, FragmentIn.FragTexCoords));
    mat.Ambient = 0.005 * mat.Diffuse;
    mat.Specular = vec3(texture(texture_specular1, FragmentIn.FragTexCoords));
    mat.Shininess = 128.0;

    vec3 normal = vec3(texture(texture_normal1, FragmentIn.FragTexCoords));
    normal = normal * 2.0 - 1.0;  
    normal = normalize(FragmentIn.TBN * normal); 

    vec3 color = calculateDirectionalLight(directionalLight, mat, normal, viewDir);
    /*
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        color += calculatePointlight(pointLights[i], mat, FragmentIn.FragPos, FragmentIn.FragNormal, viewDir);
    for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        color += calculateSpotLight(spotLights[i], mat, FragmentIn.FragPos, FragmentIn.FragNormal, viewDir);
    */
    float gamma = 2.2;
    o_Fragment = vec4(pow(color, vec3(1.0/gamma)), 1.0);
}