#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference : require

layout(location = 0) in vec2 inUv;
layout(location = 1) in vec4 inColor;
layout(location = 2) in flat uint inTextureIndex;
layout(location = 3) in flat uint inIsText;

layout(location = 0) out vec4 outColor;

// Bindless texture array, bound once per frame at set = 0, binding = 0.
layout(set = 0, binding = 0) uniform sampler2D bindlessTextures[];

layout(push_constant) uniform PushConstants {
    uint64_t _uiInstanceBufferAddress;
    vec2  screenSize;
    float msdfPxRange;
} pc;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(sampler2D atlas, vec2 uv) {
    vec2 unitRange = vec2(pc.msdfPxRange) / vec2(textureSize(atlas, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(uv);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    if (inIsText == 1u) {
        vec3 msd = texture(
            bindlessTextures[nonuniformEXT(inTextureIndex)],
            inUv
        ).rgb;

        float sd = median(msd.r, msd.g, msd.b);
        float screenPxDistance =
            screenPxRange(bindlessTextures[nonuniformEXT(inTextureIndex)], inUv)
            * (sd - 0.5);

        float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
        outColor = vec4(inColor.rgb, inColor.a * opacity);
    } else {
        vec4 sampled = texture(
            bindlessTextures[nonuniformEXT(inTextureIndex)],
            inUv
        );

        outColor = sampled * inColor;
    }

    if (outColor.a <= 0.001) {
        discard;
    }
}
