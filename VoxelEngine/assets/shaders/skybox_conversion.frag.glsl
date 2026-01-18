#version 450 core

layout (location = 0) out vec4 o_Color;

in o_Vertex
{
    vec3 LocalPosition;
} i_Fragment;

uniform sampler2D u_EquirectangularMap;

const vec2 InvAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= InvAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(i_Fragment.LocalPosition));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;

    o_Color = vec4(color, 1.0);
}