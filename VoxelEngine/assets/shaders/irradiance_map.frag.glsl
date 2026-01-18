#version 450 core

const float PI = 3.14159265359;

layout (location = 0) out vec4 o_Color;

in VertexOut
{
    vec3 LocalPosition;
} i_Fragment;

uniform samplerCube u_EnvironmentMap;

void main()
{
    vec3 N = normalize(i_Fragment.LocalPosition);

    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float sampleDelta = 0.025;
    float sampleNumber = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(u_EnvironmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            ++sampleNumber;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(sampleNumber));

    o_Color = vec4(irradiance, 1.0);
}