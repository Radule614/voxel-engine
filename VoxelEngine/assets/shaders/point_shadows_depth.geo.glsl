#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 u_ShadowMatrices[6];

out o_Geometry
{
    vec4 FragPosition;
} o_GeometryOut;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;

        for (int i = 0; i < 3; ++i)
        {
            o_GeometryOut.FragPosition = gl_in[i].gl_Position;
            gl_Position = u_ShadowMatrices[face] * o_GeometryOut.FragPosition;
            EmitVertex();
        }

        EndPrimitive();
    }
}