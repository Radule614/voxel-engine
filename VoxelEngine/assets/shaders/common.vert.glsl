struct VertexData
{
    vec4 Position;
    vec3 Normal;
    vec3 Tangent;
};

mat4 CalculateSkinModelMatrix()
{
    mat4 model = mat4(0.0);

    model += u_JointMatrices[int(i_Joints.x)] * i_Weights.x;
    model += u_JointMatrices[int(i_Joints.y)] * i_Weights.y;
    model += u_JointMatrices[int(i_Joints.z)] * i_Weights.z;
    model += u_JointMatrices[int(i_Joints.w)] * i_Weights.w;

    return model;
}

vec4 CalculateWorldPosition()
{
    mat4 model = u_IsSkinned ? CalculateSkinModelMatrix() : u_Model;

    return model * vec4(i_Position, 1.0);
}

VertexData CalculateWorldVertexData()
{
    mat4 model = u_IsSkinned ? CalculateSkinModelMatrix() : u_Model;

    VertexData data;
    data.Position = model * vec4(i_Position, 1.0);
    data.Normal = mat3(model) * i_Normal;
    data.Tangent = mat3(model) * i_Tangent.xyz;

    return data;
}