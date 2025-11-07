#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D fontAtlas;

// MSDF (Multi-channel Signed Distance Field) median function
float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    // Sample from MSDF atlas (RGB channels contain distance field data)
    vec3 msd = texture(fontAtlas, fragTexCoord).rgb;
    
    // Calculate signed distance from the median of the three channels
    float sigDist = median(msd.r, msd.g, msd.b);
    
    // MSDF threshold: values > 0.5 are inside the glyph, < 0.5 are outside
    // Use a tight smoothstep centered at 0.5 for sharp edges with antialiasing
    float opacity = smoothstep(0.5 - fwidth(sigDist), 0.5 + fwidth(sigDist), sigDist);
    
    // Apply color and alpha
    outColor = vec4(fragColor.rgb, fragColor.a * opacity);
}