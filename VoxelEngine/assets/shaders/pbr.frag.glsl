#version 450 core

#define MAX_POINT_LIGHTS 32

const float PI = 3.14159265359;

layout (location = 0) out vec4 o_Color;

in o_Vertex
{
    vec3 FragNormal;
    vec3 FragPosition;
    vec2 FragTexCoords;
} i_Fragment;

struct PointLight
{
    vec3 LightPosition;
    vec3 LightColor;
};

uniform vec3 u_CameraPosition;

// Material stuff
uniform bool u_HasAlbedoTexture;
uniform vec4 u_AlbedoColor;
uniform sampler2D u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AmbientOcclusion;

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

vec3 FresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

vec4 GetAlbedo();

void main()
{
    vec4 albedoVec4 = GetAlbedo();
    vec3 albedo = vec3(albedoVec4);
    float alpha = albedoVec4.a;

    if (alpha < 0.99) {
        discard;
    }

    vec3 albedoVec3 = vec3(albedo);

    vec3 N = normalize(i_Fragment.FragNormal);
    vec3 V = normalize(u_CameraPosition - i_Fragment.FragPosition);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoVec3, u_Metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < u_PointLightCount; ++i)
    {
        vec3 L = normalize(u_PointLights[i].LightPosition - i_Fragment.FragPosition);
        vec3 H = normalize(V + L);

        float distance = length(u_PointLights[i].LightPosition - i_Fragment.FragPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_PointLights[i].LightColor * attenuation;

        float NDF = DistributionGGX(N, H, u_Roughness);
        float G = GeometrySmith(N, V, L, u_Roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - u_Metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedoVec3 / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedoVec3 * u_AmbientOcclusion;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    o_Color = vec4(color, alpha);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denum = NdotV * (1.0 - k) + k;

    return num / denum;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec4 GetAlbedo()
{
    vec4 albedo;

    if (u_HasAlbedoTexture)
    {
        albedo = texture(u_Albedo, i_Fragment.FragTexCoords);
    }
    else
    {
        albedo = u_AlbedoColor;
    }

    return albedo;
}