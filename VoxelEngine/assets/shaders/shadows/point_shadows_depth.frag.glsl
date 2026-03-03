#version 450 core

in o_Geometry
{
    vec4 FragPosition;
} i_Fragment;

uniform vec3 u_LightPosition;
uniform float u_FarPlane;

void main()
{
    float lightDistance = length(i_Fragment.FragPosition.xyz - u_LightPosition);
    lightDistance = lightDistance / u_FarPlane;

    gl_FragDepth = lightDistance;
}