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

struct Material
{
    vec3 Albedo;
    float Alpha;
    float Metallic;
    float Rougness;
    float AmbientOcclusion;
};

uniform vec3 u_CameraPosition;

// Materials
uniform bool u_HasAlbedoTexture;
uniform vec4 u_AlbedoFactor;
uniform sampler2D u_AlbedoTexture;

uniform bool u_HasMetallicRoughnessTexture;
uniform sampler2D u_MetallicRoughnessTexture;
uniform float u_MetallicFactor;
uniform float u_RoughnessFactor;

uniform bool u_HasAmbientOcclusionTexture;
uniform sampler2D u_AmbientOcclusionTexture;
uniform float u_AmbientOcclusionStrength;

// Lights
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

vec3 FresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

vec4 GetAlbedo();
vec2 GetMatallicRougness();
float GetAmbientOcclusion();
vec4 CalculateColor(Material material);

void main()
{
    vec4 albedo = GetAlbedo();
    vec2 metallicRougness = GetMatallicRougness();
    float ambientOcclusion = GetAmbientOcclusion();

    if (albedo.a < 0.99) {
        discard;
    }

    Material material;
    material.Albedo = vec3(albedo);
    material.Alpha = albedo.a;
    material.Metallic = metallicRougness.x;
    material.Rougness = metallicRougness.y;
    material.AmbientOcclusion = ambientOcclusion;

    o_Color = CalculateColor(material);
}

vec4 CalculateColor(Material material)
{
    vec3 N = normalize(i_Fragment.FragNormal);
    vec3 V = normalize(u_CameraPosition - i_Fragment.FragPosition);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.Albedo, material.Metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < u_PointLightCount; ++i)
    {
        vec3 L = normalize(u_PointLights[i].LightPosition - i_Fragment.FragPosition);
        vec3 H = normalize(V + L);

        float distance = length(u_PointLights[i].LightPosition - i_Fragment.FragPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_PointLights[i].LightColor * attenuation;

        float NDF = DistributionGGX(N, H, material.Rougness);
        float G = GeometrySmith(N, V, L, material.Rougness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.Metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * material.Albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * material.Albedo * material.AmbientOcclusion;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    return vec4(color, material.Alpha);
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
    vec4 albedo = u_AlbedoFactor;

    if (u_HasAlbedoTexture)
    {
        albedo = texture(u_AlbedoTexture, i_Fragment.FragTexCoords);
    }

    return albedo;
}

vec2 GetMatallicRougness()
{
    float metallic = u_MetallicFactor;
    float rougness = u_RoughnessFactor;

    if (u_HasMetallicRoughnessTexture)
    {
        vec4 metallicRougnessTexture = texture(u_MetallicRoughnessTexture, i_Fragment.FragTexCoords);

        metallic = metallicRougnessTexture.b;
        rougness = metallicRougnessTexture.g;
    }

    return vec2(metallic, rougness);
}

float GetAmbientOcclusion()
{
    float ambientOcclusion = 0.0f;

    if (u_HasAmbientOcclusionTexture)
    {
        ambientOcclusion = texture(u_AmbientOcclusionTexture, i_Fragment.FragTexCoords).r;
        ambientOcclusion *= u_AmbientOcclusionStrength;
    }

    return ambientOcclusion;
}