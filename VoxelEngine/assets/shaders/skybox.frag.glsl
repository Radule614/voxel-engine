#version 450 core

out vec4 o_Color;

in o_Vertex
{
    vec3 WorldPosition;
} i_Fragment;

uniform samplerCube u_EnvironmentMap;

void main()
{
    vec3 envColor = texture(u_EnvironmentMap, i_Fragment.WorldPosition).rgb;

    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

//    o_Color = vec4(1.0, 0.0, 0.0, 1.0);
    o_Color = vec4(envColor, 1.0);
}