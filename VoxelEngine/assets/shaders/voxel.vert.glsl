#version 450 core

layout (location = 0) in vec3 i_Position;
layout (location = 1) in uint i_RadianceBaseIndex;
layout (location = 2) in uint i_Face;
layout (location = 3) in vec2 i_TexCoords;

layout (std430, binding = 0) buffer RadianceData {
    float i_Radiance[];
};

out o_Vertex {
    flat float Radiance;
    vec2 FragTexCoords;
} o_VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

const int RADIANCE_WIDTH = 18;
const int RADIANCE_HEIGHT = 130;

int GetRadianceArrayIndex(int x, int z, int y) {
    return x * RADIANCE_WIDTH * RADIANCE_HEIGHT + z * RADIANCE_HEIGHT + y;
}

// TODO: Clean this method
int CalculateNeighbourRadianceIndex() {
    int radianceIndex = int(i_RadianceBaseIndex);

    switch (i_Face) {
        case 0:
            radianceIndex += GetRadianceArrayIndex(0, 0, 1);
            break;
        case 1:
            radianceIndex += GetRadianceArrayIndex(0, 0, -1);
            break;
        case 2:
            radianceIndex += GetRadianceArrayIndex(0, 1, 0);
            break;
        case 3:
            radianceIndex += GetRadianceArrayIndex(1, 0, 0);
            break;
        case 4:
            radianceIndex += GetRadianceArrayIndex(0, -1, 0);
            break;
        case 5:
            radianceIndex += GetRadianceArrayIndex(-1, 0, 0);
            break;
    }

    return radianceIndex;
}

void main() {
    vec4 modelPos = u_Model * vec4(i_Position, 1.0);

    o_VertexOut.FragTexCoords = i_TexCoords;

    o_VertexOut.Radiance = i_Radiance[CalculateNeighbourRadianceIndex()];

    gl_Position = u_ViewProjection * modelPos;
}