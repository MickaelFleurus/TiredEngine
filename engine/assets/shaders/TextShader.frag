#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in flat uint fragTextureIndex;
layout(location = 3) in flat uint fragMaterialIndex;

layout(location = 0) out vec4 outColor;

void main() {
    // Output bright GREEN regardless of anything else
    outColor = vec4(0.0, 1.0, 0.0, 1.0);
}