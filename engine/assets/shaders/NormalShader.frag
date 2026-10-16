#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;
layout(location = 3) flat in uint vTextureIndex;


layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[];

void main() {

    vec3 normalColor = vNormal * 0.5 + 0.5;

    vec4 albedo = texture(
        textures[nonuniformEXT(vTextureIndex)],
        vUV
    );

    outColor = vec4(normalColor, 1.0) * albedo * vColor;
}

