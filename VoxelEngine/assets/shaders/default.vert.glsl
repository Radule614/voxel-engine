#version 450 core

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_TexCoord;

out o_Vertex {
    vec3 FragNormal;
    vec3 FragPos;
    vec2 FragTexCoords;
} o_VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main() {
    vec4 modelPos = u_Model * vec4(i_Position, 1.0);
    gl_Position = u_ViewProjection * modelPos;

    o_VertexOut.FragNormal = i_Normal;
    o_VertexOut.FragTexCoords = i_TexCoord;
    o_VertexOut.FragPos = vec3(modelPos);
}
