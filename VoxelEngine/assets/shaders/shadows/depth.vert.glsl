#version 450 core

layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Normal;
layout (location = 3) in vec4 i_Tangent;
layout (location = 4) in vec4 i_Joints;
layout (location = 5) in vec4 i_Weights;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;

uniform mat4 u_JointMatrices[128];
uniform bool u_IsSkinned;

#include "common.vert.glsl"

void main()
{
    vec4 worldPosition = CalculateWorldPosition();

    gl_Position = u_LightSpaceMatrix * worldPosition;
}