#version 450 core

layout (location = 0) out vec4 o_Color;

in o_Vertex {
    flat int Radiance;
    vec2 FragTexCoords;
} i_Fragment;

uniform sampler2D u_Atlas;
uniform int u_MaxRadiance;

float normalizeRadiance(int v, float k, float minVal) {
    // normalize to [0,1]
    float x = float(v) / u_MaxRadiance;

    // flipped exponential, in [0, ~1)
    float expCurve = 1.0 - exp(-k * x);

    // normalize so expCurve ends at exactly 1.0
    float normalized = expCurve / (1.0 - exp(-k));

    // remap to [minVal, 1.0]
    return minVal + (1.0 - minVal) * normalized;
}

void main() {
    vec4 texColor = texture(u_Atlas, i_Fragment.FragTexCoords);

    if (texColor.a < 0.5) discard;

    float radiance = normalizeRadiance(max(0, i_Fragment.Radiance), 0.15, 0.0);

    vec4 color = vec4(texColor.xyz * radiance, texColor.a);

    o_Color = color;
}
