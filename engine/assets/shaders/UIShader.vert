#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_shader_explicit_arithmetic_types : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout : require

// ---- Per-instance UI data (sprites and text glyphs share this layout) ----
struct ScreenQuadInstance {
    vec2 posMin;
    vec2 posMax;
    vec2 uvMin;
    vec2 uvMax;
    vec4 color;
    vec2 scale;
    vec2 anchor;
    uint texIndex;
    uint isText; 
    float rotation;
    uint _pad;
};

layout(buffer_reference, std430) readonly buffer ScreenQuadInstanceBuffer {
    ScreenQuadInstance instances[];
};

layout(push_constant) uniform PushConstants {
    ScreenQuadInstanceBuffer instanceBuffer;
    vec2  screenSize;
    float msdfPxRange;
} pc;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec4 outColor;
layout(location = 2) out flat uint outTextureIndex;
layout(location = 3) out flat uint outIsText;

// Unit quad corners, two triangles, generated from vertex index alone —
// no vertex/index buffer bound for the UI pass.
const vec2 kCorners[6] = vec2[](
    vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0),
    vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0)
);

void main() {
    ScreenQuadInstance inst = pc.instanceBuffer.instances[gl_InstanceIndex];
    vec2 corner = kCorners[gl_VertexIndex];

    vec2 size = inst.posMax - inst.posMin;
    vec2 pivot = mix(inst.posMin, inst.posMax, inst.anchor);
    vec2 local = (corner - inst.anchor) * size * inst.scale;

    // Rotate around the anchor point.
    float c = cos(inst.rotation);
    float s = sin(inst.rotation);
    vec2 rotated = vec2(local.x * c - local.y * s, local.x * s + local.y * c);

    vec2 screenPos = pivot + rotated;
    vec2 ndc = vec2(
        screenPos.x / pc.screenSize.x * 2.0 - 1.0,
        1.0 - screenPos.y / pc.screenSize.y * 2.0
    );
    gl_Position = vec4(ndc, 0.0, 1.0);

    // uvRect maps the unit quad corner into the atlas sub-rect for this glyph/sprite.
    outUv = mix(inst.uvMin, inst.uvMax, corner);
    outColor = inst.color;
    outTextureIndex = inst.texIndex;
    outIsText = inst.isText;
}
