#version 450 core

#define MAX_POINT_LIGHTS 16

const float PI = 3.14159265359;

layout (location = 0) out vec4 o_Color;

in o_Vertex
{
    vec3 FragNormal;
    vec2 FragTexCoords;
    vec3 FragPosition;
    mat3 TBN;
    vec4 FragLightSpacePosition;
} i_Fragment;

struct PointLight
{
    vec3 LightPosition;
    vec3 LightColor;
};

struct DirectionalLight
{
    vec3 LightDirection;
    vec3 LightColor;
};

struct Material
{
    vec3 Albedo;
    float Alpha;
    float Metallic;
    float Rougness;
    float AmbientOcclusion;
    vec3 Normal;
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

uniform bool u_HasNormalTexture;
uniform sampler2D u_NormalTexture;
uniform float u_NormalScale;

// Lights
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;
uniform float u_ShadowFarPlane;
uniform samplerCube u_DepthMaps[MAX_POINT_LIGHTS];

uniform DirectionalLight u_DirectionalLight;
uniform sampler2D u_DepthMap;

vec3 GridSamplingDisk[20] = vec3[]
(
vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

vec3 FresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

vec4 GetAlbedo();
vec2 GetMatallicRougness();
float GetAmbientOcclusion();
vec3 GetNormal();

vec4 CalculateColor(Material material);

float CalculatePointShadow(vec3 fragPos, int lightIndex);
float CalculateDirectionalShadow(vec4 fragPosLightSpace);

void main()
{
    vec4 albedo = GetAlbedo();
    vec2 metallicRougness = GetMatallicRougness();
    float ambientOcclusion = GetAmbientOcclusion();
    vec3 normal = GetNormal();

    if (albedo.a < 0.99) {
        discard;
    }

    Material material;
    material.Albedo = albedo.xyz;
    material.Alpha = albedo.a;
    material.Metallic = metallicRougness.x;
    material.Rougness = metallicRougness.y;
    material.AmbientOcclusion = ambientOcclusion;
    material.Normal = normal;

    float shadow = 0.5;
    shadow = min(shadow, CalculateDirectionalShadow(i_Fragment.FragLightSpacePosition));
    for (int i = 0; i < u_PointLightCount; ++i)
    {
        shadow = min(shadow, CalculatePointShadow(i_Fragment.FragPosition, i));
    }

    vec4 color = CalculateColor(material);

    o_Color = vec4((1.0 - shadow) * color.xyz, color.a);
}

vec3 CalculatePbr(Material material, vec3 radiance, vec3 L, vec3 V, vec3 N, vec3 F0)
{
    vec3 H = normalize(V + L);

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
    return (kD * material.Albedo / PI + specular) * radiance * NdotL;
}

vec4 CalculateColor(Material material)
{
    vec3 N = normalize(material.Normal);
    vec3 V = normalize(u_CameraPosition - i_Fragment.FragPosition);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.Albedo, material.Metallic);

    vec3 Lo = vec3(0.0);

    // Drectional light
    vec3 L = normalize(-u_DirectionalLight.LightDirection);
    vec3 radiance = u_DirectionalLight.LightColor * 1.2;
    Lo += CalculatePbr(material, radiance, L, V, N, F0);

    // Pointlights
    for (int i = 0; i < u_PointLightCount; ++i)
    {
        vec3 L = normalize(u_PointLights[i].LightPosition - i_Fragment.FragPosition);
        float distance = length(u_PointLights[i].LightPosition - i_Fragment.FragPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_PointLights[i].LightColor * attenuation;

        Lo += CalculatePbr(material, radiance, L, V, N, F0);
    }

    vec3 ambient = vec3(0.03) * material.Albedo * material.AmbientOcclusion;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    return vec4(color, material.Alpha);
}

float CalculateDirectionalShadow(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_DepthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(i_Fragment.FragNormal);
    vec3 lightDir = normalize(-u_DirectionalLight.LightDirection);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_DepthMap, 0));
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_DepthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;
}

float CalculatePointShadow(vec3 fragPos, int lightIndex)
{
    vec3 fragToLight = fragPos - u_PointLights[lightIndex].LightPosition;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.2;
    int samples = 20;
    float viewDistance = length(u_CameraPosition - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_ShadowFarPlane)) / 25.0;

    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(u_DepthMaps[lightIndex], fragToLight + GridSamplingDisk[i] * diskRadius).r;
        closestDepth *= u_ShadowFarPlane;

        if (currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);

    return shadow;
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
        albedo *= texture(u_AlbedoTexture, i_Fragment.FragTexCoords);
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

        metallic *= metallicRougnessTexture.b;
        rougness *= metallicRougnessTexture.g;
    }

    rougness = clamp(rougness, 0.04, 1.0);

    return vec2(metallic, rougness);
}

float GetAmbientOcclusion()
{
    float ambientOcclusion = 1.0f;

    if (u_HasAmbientOcclusionTexture)
    {
        ambientOcclusion = texture(u_AmbientOcclusionTexture, i_Fragment.FragTexCoords).r;
        ambientOcclusion *= u_AmbientOcclusionStrength;
    }

    return ambientOcclusion;
}

vec3 GetNormal()
{
    if (!u_HasNormalTexture) {
        return i_Fragment.FragNormal;
    }

    vec3 normalTS = texture(u_NormalTexture, i_Fragment.FragTexCoords).xyz * 2.0 - 1.0;
    normalTS.xy *= u_NormalScale;
    normalTS = normalize(normalTS);

    return normalize(i_Fragment.TBN * normalTS);
}