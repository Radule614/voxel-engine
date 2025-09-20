#version 450 core

layout (location = 0) out vec4 o_Color;

in o_Vertex {
    flat int Radiance;
    flat int FragmentHeight;
    vec2 FragTexCoords;
} i_Fragment;

uniform sampler2D u_Atlas;
uniform int u_MaxRadiance;
uniform int u_RadianceGridWidth;
uniform int u_RadianceGridHeight;

float normalizeRadiance(int radiance, float ambient, float maxRadiance) {
    float normalized = float(radiance) / u_MaxRadiance;

    if (normalized < 1) {
        normalized -= 0.15;
    }

    return ambient + (maxRadiance - ambient) * normalized;
}

float calculateLighting() {
    float normalizedRadiance = normalizeRadiance(max(0, i_Fragment.Radiance), 0, 0.8);
    float radianceFromHeight = float(i_Fragment.FragmentHeight - 48) / float(u_RadianceGridHeight);
    return clamp(normalizedRadiance + radianceFromHeight, 0.25, 1.0);
}

void main() {
    vec4 texColor = texture(u_Atlas, i_Fragment.FragTexCoords);

    if (texColor.a < 0.5) discard;

    float lightLevel = calculateLighting();

    o_Color = vec4(texColor.xyz * lightLevel, texColor.a);
}
