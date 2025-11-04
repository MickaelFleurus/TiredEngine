#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D fontAtlas;

void main() {
    // Sample from font atlas
    float alpha = texture(fontAtlas, fragTexCoord).r;  // Assuming single-channel atlas
    
    // Apply color and alpha
    outColor = vec4(fragColor.rgb, fragColor.a * alpha);
}