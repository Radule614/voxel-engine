#version 450 core

layout (location = 0) in vec3 i_Position;

out o_Vertex
{
    vec3 WorldPosition;
} o_VertexOut;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    o_VertexOut.WorldPosition = i_Position;

    mat4 rootView = mat4(mat3(u_View));
    vec4 clipPos = u_Projection * rootView * vec4(o_VertexOut.WorldPosition, 1.0);

    gl_Position = clipPos.xyww;
}