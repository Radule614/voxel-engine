#version 450 core

layout(location = 0) in vec3 i_Position;
layout(location = 1) in uint i_VoxelIndex;
layout(location = 2) in uint i_Face;
layout(location = 3) in vec2 i_TexCoords;

layout(std430, binding = 0) buffer RadianceData {
    float radiance[];
};

out o_Vertex {
    flat uint VoxelIndex;
    flat uint Face;
    vec2 FragTexCoords;
} o_VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

int idx(int x, int z, int y) {
    return x * 16 * 128 + z * 128 + y;
}

void main() {
    vec4 modelPos = u_Model * vec4(i_Position, 1.0);

    o_VertexOut.FragTexCoords = i_TexCoords;
    o_VertexOut.Face = i_Face;
    o_VertexOut.VoxelIndex = i_VoxelIndex;

    gl_Position = u_ViewProjection * modelPos;
}
