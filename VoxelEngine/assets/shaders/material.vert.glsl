#version 450 core

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_TexCoords;
layout(location = 3) in vec3 i_Tangent;

out o_Vertex {
	vec3 FragNormal;
    vec3 FragPos;
    vec2 FragTexCoords;
    mat3 TBN;
} VertexOut;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;


void main() {
	vec4 pos = u_Model * vec4(i_Position, 1.0f);

	vec3 normal = i_Normal;
	vec3 tangent = i_Tangent;

	vec3 T = normalize(vec3(u_Model * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(u_Model * vec4(normal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	VertexOut.FragPos = vec3(pos);
	VertexOut.FragTexCoords = i_TexCoords;
	VertexOut.FragNormal = normal;
	VertexOut.TBN = mat3(T, B, N);

	gl_Position = u_ViewProjection * pos;
}